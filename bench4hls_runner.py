#!/usr/bin/env python3
"""Bench4HLS CLI entrypoint."""

from __future__ import annotations

import argparse
import json
import logging
import shutil
import sys
import time
from dataclasses import dataclass
from pathlib import Path

from bench4hls.settings import BenchConfig, load_config

### Logging ###

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s  %(levelname)-8s  %(name)s — %(message)s",
    datefmt="%H:%M:%S",
)
logger = logging.getLogger("bench4hls")
logging.getLogger("numexpr").setLevel(logging.WARNING)
logging.getLogger("numexpr.utils").setLevel(logging.WARNING)

SCRIPT_DIR    = Path(__file__).parent.resolve()
BENCHMARK_DIR = SCRIPT_DIR / "benchmark"
TB_DIR        = BENCHMARK_DIR / "testbenches"
TCL_DIR       = SCRIPT_DIR / "tcl"

_backend_instance = None # Reused across Pass@K inner loop

### CLI ###

@dataclass
class CLIArgs:
    backend: str
    pass_at_k: int
    verbose: bool
    skip_gen: bool
    skip_eval: bool
    ppa_evaluation: bool


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description="Bench4HLS — edit bench4hls_config.json for model, paths, and tool options.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    p.add_argument(
        "--backend",
        choices=["huggingface", "ollama"],
        default=None,
        help="Inference backend (required unless --skip-gen)",
    )
    p.add_argument(
        "--pass@k",
        dest="pass_at_k",
        type=int,
        default=1,
        metavar="K",
        help="Pass@K: independent generations per problem (default: 1)",
    )
    p.add_argument(
        "--skip-gen",
        action="store_true",
        help="Skip LLM; load JSON from config `output_json` if set, else "
             "<output_dir>/output_<model_slug>.json (Pass@K>1 needs `output_json` base in config)",
    )
    p.add_argument("--skip-eval", action="store_true",
                   help="Stop after extraction; no Vitis/Vivado")
    p.add_argument("--ppa-evaluation", action="store_true",
                   help="Also run reference designs → report_ref.csv + PPA comparison")
    p.add_argument("-v", "--verbose", action="store_true")
    return p


def model_slug(model: str) -> str:
    return model.replace("/", "_").replace(":", "_").replace(".", "_")

### Stage 1 — Generate ###

def stage_generate(
    cli: CLIArgs,
    cfg: BenchConfig,
    output_json: Path,
    run_idx: int,
) -> list:
    global _backend_instance
    from bench4hls.backends import HuggingFaceBackend, OllamaBackend

    logger.info("=== STAGE 1: Generate (run %d/%d) ===", run_idx, cli.pass_at_k)

    with open(cfg.input_prompts, encoding="utf-8") as f:
        prompts = json.load(f)

    if _backend_instance is None:
        logger.info("Loading backend: %s  model: %s", cli.backend, cfg.model)
        if cli.backend == "huggingface":
            _backend_instance = HuggingFaceBackend(
                model_name=cfg.model,
                max_new_tokens=cfg.max_new_tokens,
                temperature=cfg.temperature,
                use_4bit=cfg.hf_use_4bit,
                max_seq_length=cfg.hf_max_seq_length,
            )
        else:
            _backend_instance = OllamaBackend(
                model_name=cfg.model,
                host=cfg.ollama_host,
                max_new_tokens=cfg.max_new_tokens,
                temperature=cfg.temperature,
                timeout=cfg.ollama_timeout,
            )

    backend = _backend_instance
    results: list = []

    if output_json.exists():
        with open(output_json, encoding="utf-8") as f:
            results = json.load(f)
        done_ids = {r["task_n"] for r in results}
        logger.info("Resuming — %d already done", len(done_ids))
    else:
        done_ids = set()

    output_json.parent.mkdir(parents=True, exist_ok=True)

    for idx, prompt in enumerate(prompts):
        task_n = prompt.get("task_n", f"Prob{idx+1:03d}")
        if task_n in done_ids:
            continue

        instruction = prompt.get("input", "")
        t0 = time.time()
        logger.info("  [%d/%d] %s", idx + 1, len(prompts), task_n)

        response = backend.generate(instruction)
        logger.debug("  → %.1fs, %d chars", time.time() - t0, len(response))

        results.append({
            "task_n":   task_n,
            "input":    instruction,
            "response": response,
        })

        with open(output_json, "w", encoding="utf-8") as f:
            json.dump(results, f, indent=2, ensure_ascii=False)

    logger.info("Generation complete → %s (%d entries)", output_json, len(results))
    return results

### Stage 2 — Extract ###

def stage_extract(results: list, workdir: Path, run_idx: int) -> None:
    from bench4hls.extractor import write_design_files

    logger.info("=== STAGE 2: Extract (run %d) ===", run_idx)
    info = write_design_files(
        output_json=results,
        workdir=workdir,
        run_index=run_idx,
        tb_src_dir=TB_DIR,
    )
    empty = sum(1 for v in info.values() if v["empty"])
    logger.info("  Wrote %d design files  (%d empty extractions)", len(info), empty)


### Stage 3 — Evaluate ###

def stage_evaluate(cli: CLIArgs, cfg: BenchConfig, workdir: Path) -> None:
    from bench4hls.evaluator import Evaluator

    logger.info("=== STAGE 3: Evaluate (Pass@%d) ===", cli.pass_at_k)
    evaluator = Evaluator(
        workdir=workdir,
        scripts_dir=TCL_DIR,
        version=cfg.xilinx_version,
        vitis_bin=cfg.vitis_bin,
        vivado_bin=cfg.vivado_bin,
        skip_power=cfg.skip_power,
        parallel_workers=cfg.parallel_workers,
        design_timeout_seconds=cfg.design_timeout_seconds,
    )
    evaluator.run(K=cli.pass_at_k)


### Stage 4 — Report ###

def _remove_per_design_artifact_dirs(workdir: Path) -> None:
    """Remove intermediate per-design result dirs after report.csv is finalized."""
    for name in ("power_out", "sim_out", "synth_out"):
        d = workdir / name
        if d.is_dir():
            shutil.rmtree(d)


def stage_report(cli: CLIArgs, workdir: Path) -> None:
    from bench4hls.report import print_summary, generate_comparison

    logger.info("=== STAGE 4: Report ===")
    rpt = workdir / "report.csv"
    if not rpt.exists():
        logger.error("report.csv not found in %s — evaluation may have failed", workdir)
        return

    try:
        print_summary(rpt, cli.pass_at_k)

        if cli.ppa_evaluation:
            ref_rpt = BENCHMARK_DIR / "report_ref.csv"
            if ref_rpt.exists():
                generate_comparison(
                    llm_csv=rpt,
                    ref_csv=ref_rpt,
                    out_csv=workdir / "report_comparison.csv",
                )
            else:
                logger.warning("benchmark/report_ref.csv not found — skipping PPA comparison")
    finally:
        _remove_per_design_artifact_dirs(workdir)


def _output_json_for_run(cfg: BenchConfig, slug: str, workdir: Path, run_idx: int, k: int) -> Path:
    if cfg.output_json is not None and k == 1:
        return cfg.output_json.resolve()
    base = cfg.output_json or (workdir / f"output_{slug}.json")
    base = base.resolve()
    if k == 1:
        return base
    return base.parent / f"{base.stem}_run{run_idx}{base.suffix}"


def main() -> None:
    global _backend_instance
    parser = build_parser()
    ns = parser.parse_args()

    if not ns.skip_gen and ns.backend is None:
        parser.error("--backend is required unless --skip-gen is set")

    cli = CLIArgs(
        backend=ns.backend or "",
        pass_at_k=max(1, ns.pass_at_k),
        verbose=bool(ns.verbose),
        skip_gen=bool(ns.skip_gen),
        skip_eval=bool(ns.skip_eval),
        ppa_evaluation=bool(ns.ppa_evaluation),
    )

    if cli.verbose:
        logging.getLogger().setLevel(logging.DEBUG)

    try:
        cfg = load_config(SCRIPT_DIR)
    except FileNotFoundError as exc:
        logger.error("%s", exc)
        sys.exit(1)

    slug = model_slug(cfg.model)

    workdir = cfg.output_dir
    if workdir is None:
        workdir = SCRIPT_DIR / f"eval_{slug}"
    workdir = workdir.resolve()
    workdir.mkdir(parents=True, exist_ok=True)
    logger.info("Working directory: %s", workdir)

    if cli.skip_gen and cfg.output_json is None and cli.pass_at_k > 1:
        logger.error("--skip-gen with Pass@K>1 requires `output_json` in bench4hls_config.json "
                     "(base path; _runN suffix added per run)")
        sys.exit(1)

    for run_idx in range(1, cli.pass_at_k + 1):
        out_json = _output_json_for_run(cfg, slug, workdir, run_idx, cli.pass_at_k)

        if cli.skip_gen:
            if not out_json.exists():
                logger.error("--skip-gen but file missing: %s", out_json)
                sys.exit(1)
            with open(out_json, encoding="utf-8") as f:
                results = json.load(f)
            logger.info("Loaded %d entries from %s", len(results), out_json)
        else:
            results = stage_generate(cli, cfg, out_json, run_idx)

        stage_extract(results, workdir, run_idx)

    if _backend_instance is not None:
        _backend_instance.close()
        _backend_instance = None

    if cli.skip_eval:
        logger.info("--skip-eval — done after extraction")
        return

    stage_evaluate(cli, cfg, workdir)
    stage_report(cli, workdir)


if __name__ == "__main__":
    main()
