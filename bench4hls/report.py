"""Parse report.csv, print Pass@K, and compare with reference."""

import csv
import logging
from pathlib import Path
from typing import Dict

logger = logging.getLogger(__name__)

_METRICS = ["Latency(ns)", "FF_UTIL(%)", "LUT_UTIL(%)", "DSP_UTIL(%)", "BRAM_UTIL(%)", "Power(W)"]


def parse_report(csv_path: Path) -> list:
    """Return list of row dicts from report.csv (skip Pass@K summary rows)."""
    rows = []
    with open(csv_path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("Design", "").startswith("Pass@"):
                continue
            if row.get("Design", "").startswith("TotalElapsed"):
                continue
            rows.append(row)
    return rows


def passk_summary(csv_path: Path, K: int) -> Dict[str, str]:
    """Extract Pass@K metrics from the summary row written by the TCL scripts."""
    with open(csv_path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            if row.get("Design", "").startswith(f"Pass@{K}"):
                return {
                    "Compilation": row.get("Compilation_Status", "N/A"),
                    "Simulation":  row.get("Simulation_Status",  "N/A"),
                    "Synthesis":   row.get("Synthesis_Status",   "N/A"),
                }
    return {}


def print_summary(csv_path: Path, K: int) -> None:
    summary = passk_summary(csv_path, K)
    if not summary:
        logger.warning("No Pass@%d row found in %s", K, csv_path)
        return
    print(f"\n{'='*45}")
    print(f"  Pass@{K} Results")
    print(f"{'='*45}")
    for metric, val in summary.items():
        print(f"  {metric:<15} {val}")
    print(f"{'='*45}\n")


def generate_comparison(
    llm_csv: Path,
    ref_csv: Path,
    out_csv: Path,
) -> None:
    """
    Compare LLM best-run metrics vs reference, write report_comparison.csv.

    Delta formulas (negative = LLM is better):
      del_Latency (ns)   = LLM_latency   - ref_latency
      del_FF_UTIL (%)    = LLM_ff_util   - ref_ff_util
      del_LUT_UTIL (%)   = LLM_lut_util  - ref_lut_util
      del_DSP_UTIL (%)   = LLM_dsp_util  - ref_dsp_util
      del_BRAM_UTIL (%)  = LLM_bram_util - ref_bram_util
      del_Power (%)      = 100 * (LLM_power - ref_power) / ref_power
    """
    try:
        import pandas as pd  # type: ignore
        import numpy as np   # type: ignore
    except ImportError:
        logger.error("pandas/numpy not installed — skipping comparison report")
        return

    ref_df = pd.read_csv(ref_csv)
    llm_df = pd.read_csv(llm_csv)

    # Normalise column names: handle both (%%) and (%) variants
    ref_df.columns = ref_df.columns.str.replace("%%", "%", regex=False)
    llm_df.columns = llm_df.columns.str.replace("%%", "%", regex=False)

    # Normalise design names
    ref_df["Design"] = ref_df["Design"].str.replace(r"_ref$", "", regex=True)
    llm_df["Design"] = llm_df["Design"].str.replace(r"_run\d+$", "", regex=True)

    # Exclude aggregate/summary rows from comparison math.
    ref_df = ref_df[~ref_df["Design"].astype(str).str.startswith("Pass@")]
    llm_df = llm_df[~llm_df["Design"].astype(str).str.startswith("Pass@")]
    ref_df = ref_df[~ref_df["Design"].astype(str).str.startswith("TotalElapsed")]
    llm_df = llm_df[~llm_df["Design"].astype(str).str.startswith("TotalElapsed")]

    metric_cols = {
        "Latency(ns)":   "Latency",
        "FF_UTIL(%)":    "FF_UTIL",
        "LUT_UTIL(%)":   "LUT_UTIL",
        "DSP_UTIL(%)":   "DSP_UTIL",
        "BRAM_UTIL(%)":  "BRAM_UTIL",
        "Power(W)":      "Power",
    }

    ref_rename = {k: v for k, v in metric_cols.items() if k in ref_df.columns}
    llm_rename = {k: v for k, v in metric_cols.items() if k in llm_df.columns}
    ref_df = ref_df.rename(columns=ref_rename)[["Design"] + list(ref_rename.values())]
    llm_df = llm_df.rename(columns=llm_rename)[["Design"] + list(llm_rename.values())]

    for col in list(metric_cols.values()):
        ref_df[col] = pd.to_numeric(ref_df[col], errors="coerce")
        llm_df[col] = pd.to_numeric(llm_df[col], errors="coerce")

    llm_valid = llm_df.dropna(subset=list(metric_cols.values()), how="all")
    best = llm_valid.groupby("Design")[list(metric_cols.values())].min().reset_index()

    cmp = pd.merge(ref_df, best, on="Design", how="left", suffixes=("_ref", "_best"))

    for m in ["Latency", "FF_UTIL", "LUT_UTIL", "DSP_UTIL", "BRAM_UTIL"]:
        cmp[f"del_{m}"] = cmp[f"{m}_best"] - cmp[f"{m}_ref"]

    cmp["del_Power"] = 100.0 * (cmp["Power_best"] - cmp["Power_ref"]) / cmp["Power_ref"]

    delta_cols = [c for c in cmp.columns if c.startswith("del_")]
    no_data_mask = cmp["Power_best"].isna()
    cmp.loc[no_data_mask, delta_cols] = np.nan

    out_df = cmp[["Design"] + delta_cols]

    # Rename columns to include units
    col_rename = {
        "del_Latency":   "del_Latency (ns)",
        "del_FF_UTIL":   "del_FF_UTIL (%)",
        "del_LUT_UTIL":  "del_LUT_UTIL (%)",
        "del_DSP_UTIL":  "del_DSP_UTIL (%)",
        "del_BRAM_UTIL": "del_BRAM_UTIL (%)",
        "del_Power":     "del_Power (%)",
    }
    out_df = out_df.rename(columns=col_rename)

    out_df.to_csv(out_csv, index=False)
    logger.info("Comparison report written → %s", out_csv)
