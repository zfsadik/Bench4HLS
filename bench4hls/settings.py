"""Load bench4hls_config.json."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Optional

# Timeout for simulation/synthesis (seconds).
DESIGN_TIMEOUT_SECONDS = 300


@dataclass
class BenchConfig:
    model: str
    input_prompts: Path
    output_dir: Optional[Path]
    output_json: Optional[Path]
    max_new_tokens: int
    temperature: float
    hf_use_4bit: bool
    hf_max_seq_length: int
    ollama_host: str
    ollama_timeout: int
    xilinx_version: str
    vitis_bin: Optional[str]
    vivado_bin: Optional[str]
    skip_power: bool
    parallel_workers: int
    design_timeout_seconds: int


def _resolve(script_dir: Path, p: Optional[str]) -> Optional[Path]:
    if p is None or p == "":
        return None
    path = Path(p)
    if not path.is_absolute():
        path = (script_dir / path).resolve()
    return path


def load_config(script_dir: Path, raw: Optional[dict[str, Any]] = None) -> BenchConfig:
    """Load config from file, or use injected dict for tests."""
    if raw is None:
        cfg_path = script_dir / "bench4hls_config.json"
        if not cfg_path.is_file():
            raise FileNotFoundError(
                f"Missing {cfg_path.name}. Copy bench4hls_config.example.json → "
                f"{cfg_path.name} and edit paths/model."
            )
        raw = json.loads(cfg_path.read_text(encoding="utf-8"))

    gen = raw.get("generation") or {}
    hf = raw.get("huggingface") or {}
    ol = raw.get("ollama") or {}
    xi = raw.get("xilinx") or {}

    model = raw.get("model")
    if not model:
        raise ValueError("bench4hls_config.json: 'model' is required")

    inp = raw.get("input_prompts") or "benchmark/input_prompts.json"

    return BenchConfig(
        model=model,
        input_prompts=_resolve(script_dir, inp) or (script_dir / inp).resolve(),
        output_dir=_resolve(script_dir, raw.get("output_dir")),
        output_json=_resolve(script_dir, raw.get("output_json")),
        max_new_tokens=int(gen.get("max_new_tokens", 30000)),
        temperature=float(gen.get("temperature", 1.0)),
        hf_use_4bit=bool(hf.get("use_4bit", True)),
        hf_max_seq_length=int(hf.get("max_seq_length", 8192)),
        ollama_host=str(ol.get("host", "http://localhost:11434")),
        ollama_timeout=int(ol.get("timeout_seconds", 600)),
        xilinx_version=str(xi.get("version", "2025.2.1")),
        vitis_bin=xi.get("vitis_bin") or None,
        vivado_bin=xi.get("vivado_bin") or None,
        skip_power=bool(xi.get("skip_power", False)),
        parallel_workers=int(raw.get("parallel_workers", 1)),
        design_timeout_seconds=DESIGN_TIMEOUT_SECONDS,
    )
