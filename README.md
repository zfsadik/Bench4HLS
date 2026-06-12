# Bench4HLS

An automated framework for evaluating LLM-generated High-Level Synthesis (HLS) C++ code against benchmark tasks using Xilinx Vitis HLS and Vivado.

## Overview

Bench4HLS evaluates generated HLS C++ designs through:
- **C-Simulation**: Functional correctness testing
- **C-Synthesis**: Resource utilization and latency analysis
- **Power Estimation**: Post-synthesis power analysis via Vivado
- **Pass@K Metrics**: Statistical evaluation across multiple generations
- **PPA Comparison**: Performance, Power, and Area comparison with reference designs

## Repository Layout

```text
bench4HLS/
├── bench4hls_runner.py
├── bench4hls_config.json
├── requirements.txt
├── bench4hls/
│   ├── backends/
│   ├── extractor.py
│   ├── evaluator.py
│   ├── report.py
│   └── settings.py
├── benchmark/
│   ├── input_prompts.json
│   ├── prompts/
│   ├── testbenches/
│   ├── reference_design/
│   └── report_ref.csv
└── tcl/
    ├── simulation.tcl
    ├── synthesis.tcl
    └── vivado_power.tcl
```

## Requirements

- Python 3.10+
- Xilinx Vitis/Vivado (tested with 2025.2.1)

Install Python dependencies:

```bash
pip install -r requirements.txt
```

## `bench4hls_config.json` Configuration

Edit the config file according to your requirement:

```json
{
  "model": "only for generation",
  "input_prompts": "benchmark/input_prompts.json",
  "output_dir": "/path to evaluation_output",
  "output_json": "output.json",
  "generation": {
    "max_new_tokens": 30000,
    "temperature": 0.8
  },
  "huggingface": {
    "use_4bit": true,
    "max_seq_length": 8192
  },
  "ollama": {
    "host": "http://localhost:11434",
    "timeout_seconds": 600
  },
  "parallel_workers": 4,
  "xilinx": {
    "vitis_bin": "/path to vivado installation dir/2025.2.1/Vitis/bin/unwrapped/lnx64.o/vitis_hls",
    "vivado_bin": "/path to vivado installation dir/2025.2.1/Vivado/bin/vivado"
  }
}
```

Fields:

- `model`: model name (Ollama tag or HF model id).
- `input_prompts`: benchmark prompt file.
- `output_dir`: evaluation workspace directory.
- `output_json`: generated raw LLM responses.
- `generation.max_new_tokens`, `generation.temperature`: generation parameters.
- `huggingface.use_4bit`, `huggingface.max_seq_length`: HF backend options.
- `ollama.host`, `ollama.timeout_seconds`: Ollama connection options.
- `parallel_workers`: number of parallel design evaluations.
- `xilinx.vitis_bin`: path to unwrapped `vitis_hls`.
- `xilinx.vivado_bin`: path to `vivado`.

## Step-by-Step Evaluation

### 1) Configure

Edit `bench4hls_config.json`:
- set `model`
- set `output_dir`
- set Xilinx paths under `xilinx`

### 2) Run a full evaluation (Pass@1)

This runs all four stages (generate, extract, evaluate, report) end-to-end.

**Ollama backend:**

```bash
python bench4hls_runner.py --backend ollama --pass@k 1 --ppa-evaluation
```

**Hugging Face backend:**

```bash
python bench4hls_runner.py --backend huggingface --pass@k 1 --ppa-evaluation
```

### 3) Run Pass@K evaluation

Pass@K generates K independent responses per problem and checks whether at least one passes.
Use `--pass@k K` where K is the number of independent samples:

```bash
python bench4hls_runner.py --backend ollama --pass@k 5 --ppa-evaluation
```

### 4) Skip LLM generation (reuse saved responses)

If you already have `output.json` from a previous run, skip generation:

```bash
python bench4hls_runner.py --skip-gen --pass@k 1 --ppa-evaluation
```

For Pass@k > 1 with `--skip-gen`, you must set `output.json` in `bench4hls_config.json` to a base path. Keep the files named `output_run1.json`, `output_run2.json`,....`output_runK.json`.

### 5) Generate PPA comparison report

After evaluation, compare LLM designs against reference designs:

```bash
python bench4hls_runner.py --skip-gen --pass@k 1 --ppa-evaluation
```

This reads `benchmark/report_ref.csv` (pre-generated reference metrics) and writes `report_comparison.csv` into `output_dir` with per-problem delta values.

### 6) Adjust parallel workers

Set `parallel_workers` in `bench4hls_config.json` to run multiple designs concurrently:

```json
"parallel_workers": 4
```

---

## CLI Reference

```
python bench4hls_runner.py [OPTIONS]
```

| Flag | Description |
| --- | --- |
| `--backend ollama\|huggingface` | LLM backend to use (required unless `--skip-gen`) |
| `--pass@k K` | Number of independent samples per problem (default: 1) |
| `--skip-gen` | Skip LLM generation; load responses from `output_json` |
| `--skip-eval` | Stop after code extraction; do not run Vitis/Vivado |
| `--ppa-evaluation` | Generate `report_comparison.csv` vs reference designs |
| `-v, --verbose` | Enable debug logging |

---

### report_comparison.csv delta formulas

For each problem, the LLM metric used is the best (minimum) value across all K runs. A blank row means the LLM design failed simulation or synthesis.

| Column | Formula |
| --- | --- |
| `del_Latency (ns)` | `LLM_Latency - Ref_Latency` |
| `del_FF_UTIL (%)` | `LLM_FF_UTIL - Ref_FF_UTIL` |
| `del_LUT_UTIL (%)` | `LLM_LUT_UTIL - Ref_LUT_UTIL` |
| `del_DSP_UTIL (%)` | `LLM_DSP_UTIL - Ref_DSP_UTIL` |
| `del_BRAM_UTIL (%)` | `LLM_BRAM_UTIL - Ref_BRAM_UTIL` |
| `del_Power (%)` | `100 * (LLM_Power - Ref_Power) / Ref_Power` |

---

## Citation

If you use Bench4HLS in your research, please cite:

```
@article{khan2026bench4hls,
  title={Bench4HLS: End-to-End Evaluation of LLMs in High-Level Synthesis Code Generation},
  author={Khan, M and Azar, Kimia and Kamali, Hadi},
  journal={arXiv preprint arXiv:2601.19941},
  year={2026}
}
```
