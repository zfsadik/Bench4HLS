"""Extract HLS C++ code from LLM responses."""

import re
import logging
from pathlib import Path
from typing import Optional

logger = logging.getLogger(__name__)

_CODE_START_RE = re.compile(
    r"^\s*(#include|#pragma|#ifndef|#define|void\s+TopModule|using\s+namespace)",
    re.MULTILINE,
)

_FENCE_RE = re.compile(
    r"```(?:cpp|c\+\+|c)?\s*\n(.*?)```",
    re.DOTALL | re.IGNORECASE,
)

_RESPONSE_MARKER = "### Response:"


def extract_hls_code(response: str) -> str:
    """Return cleaned C++ code (may still be empty if model gave nothing useful)"""
    blocks = _FENCE_RE.findall(response)
    if blocks:
        # Prefer the longest block — most likely to be the full design
        code = max(blocks, key=len).strip()
        return _clean(code)

    marker_idx = response.rfind(_RESPONSE_MARKER)
    if marker_idx != -1:
        tail = response[marker_idx + len(_RESPONSE_MARKER):]
        extracted = _scan_for_code(tail)
        if extracted:
            return _clean(extracted)

    extracted = _scan_for_code(response)
    if extracted:
        return _clean(extracted)

    logger.warning("Could not locate code block — returning raw tail of response")
    tail = response.split(_RESPONSE_MARKER, 1)[-1] if _RESPONSE_MARKER in response else response
    return _clean(tail)


def _scan_for_code(text: str) -> Optional[str]:
    """Find first code-start indicator and slice to the last closing brace."""
    m = _CODE_START_RE.search(text)
    if not m:
        return None
    code_start = m.start()
    snippet = text[code_start:]
    last_brace = snippet.rfind("}")
    if last_brace == -1:
        return snippet
    return snippet[: last_brace + 1]


def _clean(code: str) -> str:
    """Strip trailing markdown noise, normalize line endings."""
    code = code.strip()
    # Remove trailing ``` or explanatory text after last }
    last_brace = code.rfind("}")
    if last_brace != -1:
        code = code[: last_brace + 1]
    return code.strip()


# File-level helpers used by the main runner
def write_design_files(
    output_json: list,
    workdir: Path,
    run_index: int,
    tb_src_dir: Path,
) -> dict:
    """Write extracted design files and copy testbenches."""
    workdir.mkdir(parents=True, exist_ok=True)
    results = {}

    for entry in output_json:
        task_n = entry.get("task_n", "")
        if not task_n:
            continue

        response = entry.get("response", "")
        code = extract_hls_code(response)

        # Design file
        design_name = f"{task_n}_design_run{run_index}.cpp"
        design_path = workdir / design_name
        if not code:
            logger.warning("%s run%d: extraction produced empty code — writing placeholder", task_n, run_index)
        design_path.write_text(code or f"// EXTRACTION FAILED for {task_n}\n", encoding="utf-8")

        # Testbench — copy once, reuse across runs
        tb_name = f"{task_n}_tb.cpp"
        tb_dest = workdir / tb_name
        if not tb_dest.exists():
            tb_src = tb_src_dir / tb_name
            if tb_src.exists():
                tb_dest.write_text(tb_src.read_text(encoding="utf-8"), encoding="utf-8")
            else:
                logger.warning("Testbench not found: %s", tb_src)

        results[task_n] = {
            "code_file": design_path,
            "tb_file": tb_dest if tb_dest.exists() else None,
            "empty": not bool(code),
        }

    return results
