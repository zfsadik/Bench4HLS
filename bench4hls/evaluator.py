"""Run Vitis HLS + Vivado evaluation pipeline."""

import logging
import os
import signal
import shutil
import subprocess
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path
from typing import Optional, Tuple

logger = logging.getLogger(__name__)

_XILINX_ROOTS = [
    Path("/tools/Xilinx"),
    Path("/opt/Xilinx"),
    Path("/usr/local/Xilinx"),
]


def _find_tool(tool: str, version: str, hint: Optional[str] = None) -> Path:
    if hint:
        p = Path(hint)
        if p.exists():
            return p
        raise FileNotFoundError(f"Explicit tool path not found: {hint}")

    which = shutil.which(tool)
    if which:
        return Path(which)

    for root in _XILINX_ROOTS:
        subfolder = "Vitis_HLS" if "vitis" in tool else "Vivado"
        candidate = root / subfolder / version / "bin" / tool
        if candidate.exists():
            return candidate

    raise FileNotFoundError(
        f"Could not locate '{tool}' for version {version}.\n"
        f"Searched: PATH and {_XILINX_ROOTS}\n"
        f"Pass --vitis / --vivado explicitly."
    )


class Evaluator:
    def __init__(
        self,
        workdir: Path,
        scripts_dir: Path,
        total_problems: int = 170,
        version: str = "2025.2.1",
        vitis_bin: Optional[str] = None,
        vivado_bin: Optional[str] = None,
        skip_power: bool = False,
        parallel_workers: int = 1,
        design_timeout_seconds: int = 300,
    ):
        self.workdir = workdir.resolve()
        self.scripts_dir = scripts_dir.resolve()
        self.total_problems = total_problems
        self.skip_power = skip_power
        self.parallel_workers = max(1, parallel_workers)
        self.design_timeout = design_timeout_seconds

        self.logs_dir = self.workdir / "logs"
        self.logs_dir.mkdir(parents=True, exist_ok=True)

        self.vitis = _find_tool("vitis_hls", version, vitis_bin)
        if not skip_power:
            self.vivado = _find_tool("vivado", version, vivado_bin)

        # Source Xilinx settings64.sh to get full environment for vitis_hls
        self.vitis_env_base = self._build_xilinx_env(self.vitis)

        logger.info("vitis_hls : %s", self.vitis)
        if not skip_power:
            logger.info("vivado    : %s", self.vivado)
        if self.parallel_workers > 1:
            logger.info("parallel_workers : %d", self.parallel_workers)

        _p = Path(self.vitis)
        if _p.exists() and not os.access(str(_p), os.X_OK):
            logger.warning("vitis binary not executable — trying chmod +x on %s", _p)
            _p.chmod(_p.stat().st_mode | 0o111)

    def _build_xilinx_env(self, vitis_bin: Path) -> dict:
        """Build env by sourcing Xilinx setup script."""
        vitis_install = vitis_bin.parent
        if "unwrapped" in str(vitis_bin):
            vitis_install = vitis_bin.parent.parent.parent.parent
        elif vitis_install.name == "bin":
            vitis_install = vitis_install.parent
        
        setup_env = vitis_install / "bin" / "setupEnv.sh"
        
        if not setup_env.exists():
            logger.warning(
                "Xilinx setupEnv.sh not found at %s — using current environment", setup_env
            )
            return os.environ.copy()

        cmd = f'cd "{vitis_install}/bin" && source ./setupEnv.sh > /dev/null 2>&1 && env'
        result = subprocess.run(
            ["bash", "-c", cmd],
            capture_output=True,
            text=True,
            check=False,
        )
        if result.returncode != 0:
            logger.warning("Failed to source setupEnv.sh — using current environment")
            return os.environ.copy()

        env = {}
        for line in result.stdout.splitlines():
            if "=" in line:
                key, _, val = line.partition("=")
                env[key] = val

        if "RDI_PLATFORM" not in env:
            env["RDI_PLATFORM"] = "lnx64"
        if "RDI_OPT_EXT" not in env:
            env["RDI_OPT_EXT"] = ".o"
        
        if "RDI_DATADIR" not in env:
            data_paths = []
            shared_data = vitis_install.parent.parent / "SharedData" / vitis_install.parent.name / "data"
            if shared_data.exists():
                data_paths.append(str(shared_data))
            vitis_data = vitis_install / "data"
            if vitis_data.exists():
                data_paths.append(str(vitis_data))
            vitis_data_vitis = vitis_install / "data" / "vitis"
            if vitis_data_vitis.exists():
                data_paths.insert(0, str(vitis_data_vitis))
            if data_paths:
                env["RDI_DATADIR"] = ":".join(data_paths)

        if "TCL_LIBRARY" not in env:
            tcl_lib = vitis_install / "tps" / "tcl" / "tcl8.6"
            if tcl_lib.exists():
                env["TCL_LIBRARY"] = str(tcl_lib)
        if "TK_LIBRARY" not in env:
            tk_lib = vitis_install / "tps" / "tk" / "tk8.5"
            if tk_lib.exists():
                env["TK_LIBRARY"] = str(tk_lib)

        ldlib_script = vitis_install / "bin" / "ldlibpath.sh"
        lib_dir = vitis_install / "lib" / "lnx64.o"
        if ldlib_script.exists() and lib_dir.exists():
            result_ld = subprocess.run(
                [str(ldlib_script), str(lib_dir)],
                capture_output=True,
                text=True,
                check=False,
            )
            if result_ld.returncode == 0:
                new_ld_path = result_ld.stdout.strip()
                existing_ld = env.get("LD_LIBRARY_PATH", "")
                if existing_ld:
                    env["LD_LIBRARY_PATH"] = f"{new_ld_path}:{existing_ld}"
                else:
                    env["LD_LIBRARY_PATH"] = new_ld_path

        logger.debug("Built Xilinx environment from %s", setup_env)
        return env

    def run(self, K: int) -> None:
        for run_idx in range(1, K + 1):
            logger.info("===== Run %d / %d =====", run_idx, K)
            self._run_simulation(K, run_idx)
            self._run_synthesis(K, run_idx)
            if not self.skip_power:
                self._run_vivado_power(run_idx)
        # Build report once after all K runs so every design×run row is included and Pass@K reflects whether any run passed per problem.
        self._build_report_csv(K)

    def _vitis_env(self, K: int, run: int, start: int, end: int) -> dict:
        env = self.vitis_env_base.copy()
        env.update({
            "DESIGN_K":     str(K),
            "DESIGN_RUN":   str(run),
            "DESIGN_START": str(start),
            "DESIGN_END":   str(end),
        })
        return env

    def _run_proc(
        self,
        cmd: list,
        cwd: str,
        env: dict,
        log_path: Path,
        timeout: Optional[float],
    ) -> Tuple[int, bool]:
        """Run command with optional timeout. Return (returncode, timed_out)."""
        logger.debug("CMD: %s", " ".join(cmd))
        with open(log_path, "w") as logf:
            proc = subprocess.Popen(
                cmd,
                cwd=cwd,
                env=env,
                stdout=logf,
                stderr=subprocess.STDOUT,
                start_new_session=True,
            )
            try:
                if timeout is None:
                    proc.wait()
                else:
                    proc.wait(timeout=timeout)
                return proc.returncode, False
            except subprocess.TimeoutExpired:
                try:
                    os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
                except (ProcessLookupError, PermissionError):
                    try:
                        proc.kill()
                    except:
                        pass
                proc.wait()
                return -1, True

    def _sim_one(self, K: int, run: int, design_n: int, design_label: str) -> int:
        logger.info("  simulation design %s", design_label)
        tcl = self.scripts_dir / "simulation.tcl"
        log = self.logs_dir / f"sim_run{run}_design{design_n}.log"
        env = self._vitis_env(K, run, design_n, design_n)
        rc, timed_out = self._run_proc(
            [str(self.vitis), "-nolog", "-nosplash", "-f", str(tcl)],
            str(self.workdir),
            env,
            log,
            timeout=float(self.design_timeout),
        )
        if timed_out:
            sim_file = self.workdir / "sim_out" / f"{design_label}.txt"
            sim_file.parent.mkdir(exist_ok=True)
            sim_file.write_text(
                f"FAIL\nFAIL\nTimeout after {self.design_timeout}s\n", encoding="utf-8"
            )
        elif rc != 0:
            logger.error("  simulation failed for %s (run %d)", design_label, run)
        return rc

    def _synth_one(self, K: int, run: int, design_n: int, design_label: str) -> int:
        logger.info("  synthesis design %s", design_label)
        tcl = self.scripts_dir / "synthesis.tcl"
        log = self.logs_dir / f"synth_run{run}_design{design_n}.log"
        env = self._vitis_env(K, run, design_n, design_n)
        rc, timed_out = self._run_proc(
            [str(self.vitis), "-nolog", "-nosplash", "-f", str(tcl)],
            str(self.workdir),
            env,
            log,
            timeout=float(self.design_timeout),
        )
        if timed_out:
            synth_file = self.workdir / "synth_out" / f"{design_label}.txt"
            synth_file.parent.mkdir(exist_ok=True)
            synth_file.write_text(f"FAIL\n\n\n\n\n\n", encoding="utf-8")
        elif rc != 0:
            logger.error("  synthesis failed for %s (run %d)", design_label, run)
        return rc

    def _vivado_one(self, run: int, design: str) -> int:
        logger.info("  vivado power design %s", design)
        tcl = self.scripts_dir / "vivado_power.tcl"
        log = self.logs_dir / f"vivado_{design}.log"
        cmd = [
            str(self.vivado), "-mode", "batch",
            "-source", str(tcl),
            "-tclargs", design,
        ]
        with open(log, "w") as logf:
            rc = subprocess.run(
                cmd,
                cwd=str(self.workdir),
                env=os.environ.copy(),
                stdout=logf,
                stderr=subprocess.STDOUT,
            ).returncode
        if rc != 0:
            logger.warning("vivado exited %d for %s — check %s", rc, design, log)
        return rc

    def _run_parallel(self, fn, args_list) -> list:
        """Run fn(*args) for each args in args_list with thread pool."""
        results = []
        if self.parallel_workers <= 1:
            for args in args_list:
                results.append(fn(*args))
        else:
            with ThreadPoolExecutor(max_workers=self.parallel_workers) as pool:
                futures = {pool.submit(fn, *args): args for args in args_list}
                for future in as_completed(futures):
                    results.append(future.result())
        return results

    def _run_simulation(self, K: int, run: int) -> None:
        files = sorted(self.workdir.glob(f"*_design_run{run}.cpp"))
        args = [
            (K, run, i + 1, src.stem.replace("_design", ""))
            for i, src in enumerate(files)
        ]
        self._run_parallel(self._sim_one, args)

    def _run_synthesis(self, K: int, run: int) -> None:
        files = sorted(self.workdir.glob(f"*_design_run{run}.cpp"))
        args = [
            (K, run, i + 1, src.stem.replace("_design", ""))
            for i, src in enumerate(files)
        ]
        self._run_parallel(self._synth_one, args)

    def _run_vivado_power(self, run: int) -> None:
        pattern = list(self.workdir.glob(f"*_design_run{run}.cpp"))
        args = []
        for src_file in sorted(pattern):
            stem = src_file.stem
            design = stem.replace("_design", "")
            args.append((run, design))
        self._run_parallel(self._vivado_one, args)

    def _build_report_csv(self, K: int) -> None:
        """Aggregate per-design outputs into report.csv and Pass@K row."""
        header = (
            "Design,Compilation_Status,Simulation_Status,Comment,"
            "Synthesis_Status,Latency(ns),FF_UTIL(%),LUT_UTIL(%),DSP_UTIL(%),BRAM_UTIL(%),Power(W)"
        )

        sim_dir   = self.workdir / "sim_out"
        synth_dir = self.workdir / "synth_out"
        power_dir = self.workdir / "power_out"

        rows: list[str] = []

        prob_comp_pass:  dict[str, bool] = {}
        prob_sim_pass:   dict[str, bool] = {}
        prob_synth_pass: dict[str, bool] = {}

        total_rows = 0

        for run in range(1, K + 1):
            cpp_files = sorted(self.workdir.glob(f"*_design_run{run}.cpp"))
            for src in cpp_files:
                design_nm = src.stem.replace("_design", "")
                prob = design_nm.rsplit("_run", 1)[0]

                prob_comp_pass.setdefault(prob, False)
                prob_sim_pass.setdefault(prob, False)
                prob_synth_pass.setdefault(prob, False)

                # Simulation result
                sim_file = sim_dir / f"{design_nm}.txt"
                comp_status = sim_status = "N/A"
                comment = ""
                if sim_file.exists():
                    parts = sim_file.read_text(encoding="utf-8").splitlines()
                    comp_status = parts[0] if len(parts) > 0 else "N/A"
                    sim_status  = parts[1] if len(parts) > 1 else "N/A"
                    comment     = parts[2] if len(parts) > 2 else ""

                # Synthesis result
                synth_file = synth_dir / f"{design_nm}.txt"
                synth_status = latency = ff = lut = dsp = bram = "N/A"
                if synth_file.exists():
                    parts = synth_file.read_text(encoding="utf-8").splitlines()
                    synth_status = parts[0] if len(parts) > 0 else "N/A"
                    latency      = parts[1] if len(parts) > 1 else "N/A"
                    ff           = parts[2] if len(parts) > 2 else "N/A"
                    lut          = parts[3] if len(parts) > 3 else "N/A"
                    dsp          = parts[4] if len(parts) > 4 else "N/A"
                    bram         = parts[5] if len(parts) > 5 else "N/A"

                # Power result: numeric only when synthesis passed; else blank
                power_file = power_dir / f"{design_nm}.txt"
                power = ""
                if synth_status == "PASS" and power_file.exists():
                    raw_p = power_file.read_text(encoding="utf-8").strip()
                    if raw_p and raw_p not in ("N/A", "ERROR"):
                        power = raw_p

                # Update best-of-K per problem
                if comp_status  == "PASS": prob_comp_pass[prob]  = True
                if sim_status   == "PASS": prob_sim_pass[prob]   = True
                if synth_status == "PASS": prob_synth_pass[prob] = True

                esc = comment.replace('"', '""')
                rows.append(
                    f'{design_nm},{comp_status},{sim_status},"{esc}",'
                    f'{synth_status},{latency},{ff},{lut},{dsp},{bram},{power}'
                )
                total_rows += 1

        # Pass@K: fraction of unique problems where at least one run passed
        total_probs = len(prob_comp_pass)

        def pct(d: dict[str, bool]) -> str:
            count = sum(1 for v in d.values() if v)
            return f"{100.0 * count / total_probs:.2f}%" if total_probs else "0.00%"

        rpt = self.workdir / "report.csv"
        with open(rpt, "w", encoding="utf-8") as f:
            f.write(header + "\n")
            for row in rows:
                f.write(row + "\n")
            f.write(
                f'Pass@{K},{pct(prob_comp_pass)},{pct(prob_sim_pass)},,'
                f'{pct(prob_synth_pass)},,,,,,\n'
            )

        comp_n  = sum(1 for v in prob_comp_pass.values()  if v)
        sim_n   = sum(1 for v in prob_sim_pass.values()   if v)
        synth_n = sum(1 for v in prob_synth_pass.values() if v)
        logger.info(
            "report.csv written: %d rows (%d runs × %d problems)  "
            "Pass@%d — Comp=%.2f%%  Sim=%.2f%%  Synth=%.2f%%",
            total_rows, K, total_probs,
            K,
            100.0 * comp_n  / total_probs if total_probs else 0,
            100.0 * sim_n   / total_probs if total_probs else 0,
            100.0 * synth_n / total_probs if total_probs else 0,
        )

    def run_reference(self, ref_design_dir: Path, tb_dir: Path, K: int = 1) -> None:
        import shutil as _sh
        self.workdir.mkdir(parents=True, exist_ok=True)
        logger.info("Running reference designs for report_ref.csv")

        for ref in sorted(ref_design_dir.glob("Prob*_ref.cpp")):
            task = ref.stem.replace("_ref", "")
            dest = self.workdir / f"{task}_design_run1.cpp"
            if not dest.exists():
                _sh.copy(ref, dest)
            tb_dest = self.workdir / f"{task}_tb.cpp"
            if not tb_dest.exists():
                tb_src = tb_dir / f"{task}_tb.cpp"
                if tb_src.exists():
                    _sh.copy(tb_src, tb_dest)

        self.run(K=1)

        rpt = self.workdir / "report.csv"
        ref_rpt = self.workdir / "report_ref.csv"
        if rpt.exists():
            _sh.copy(rpt, ref_rpt)
            logger.info("Reference report saved → %s", ref_rpt)
