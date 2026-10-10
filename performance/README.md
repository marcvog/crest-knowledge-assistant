# Extraction profiling and analysis

Install optional tooling from the repository root:

```bash
uv sync --locked --group performance
```

Collect a CPU and memory profile (use a fresh output directory for each run):

```bash
mkdir -p performance/results/baseline
uv run --group performance scalene run --memory --profile-all \
  --profile-only src/crest_knowledge_assistant --cpu-percent-threshold 0 \
  -o performance/results/baseline/profile.json \
  src/crest_knowledge_assistant/performance/workload.py --- \
  --input-dir performance/fixtures \
  --output-dir performance/results/baseline --repetitions 3000
```

This exercises the real Tree-sitter parser and entity extractor on fixed C++
inputs, including file reads and fresh extraction state on every iteration.
It does not save indexes, call embedding services, or connect to a database.
The workload temporarily sets extraction path roots within its dedicated process
and restores them on completion. Standard output goes to the null device;
formatting and print-call overhead are still included. Imports/startup are
included in the overall Scalene profile, while metadata's workload_seconds
covers the repeated extraction loop. Repeated reads will usually use OS caches.

The supplied tiny fixture is a smoke workload, not a representative production
corpus. Use a fixed larger input directory to investigate realistic behavior.
Supported file suffixes are .h, .cxx, and .cpp; syntax errors fail the workload.
Increase repetitions if there are too few samples. Memory is sampled, so zero
reported allocations does not prove a line allocates nothing.

Each run produces profile.json and metadata.json. Metadata includes commit,
dirty state, dataset content hash, input size, repetitions, tool version, Python,
and machine details. Keep a clean checkout for reproducible comparisons; a dirty
flag alone cannot reconstruct uncommitted source changes. Preserve raw profiles
and record any nonstandard profiler flags alongside them.

Inspect the native Scalene report:

```bash
uv run --group performance scalene view --cli performance/results/baseline/profile.json
```

Export CSV tables and a standalone CPU hotspot PNG:

```bash
uv run --group performance python -m crest_knowledge_assistant.performance.analysis performance/results/baseline
```

Use the same loader in Python or a notebook:

```python
from pathlib import Path
from crest_knowledge_assistant.performance.analysis import load_run

runs, lines = load_run(Path("performance/results/baseline"))
hotspots = lines.sort_values("cpu_python_percent", ascending=False).head(10)
print(hotspots[["file", "line_number", "cpu_python_percent", "peak_mb"]])
```

The loader targets Scalene 2.x, validated against installed version 2.3.0.
It preserves absent measurements as missing values, including memory columns
for CPU-only profiles. Column suffixes describe units: percent, Scalene-reported
MB, and MB/second. Peak memory values should not be summed across lines. CPU
percentages are relative attribution, not absolute durations. Line numbers can
change between commits; compare corresponding functions/source deliberately.
Profiles add measurement overhead and are not unprofiled benchmark timings.

Run the analysis tests with optional dependencies installed:

```bash
uv run --group performance pytest -q
```

Generated results are ignored by Git. Profiling is an explicit local operation;
normal application startup, commit hooks, and CI do not launch Scalene.

## Repeatable timings with pytest-benchmark

The performance group also includes pytest-benchmark. The three offline
benchmarks in `performance/benchmarks/` measure parsing, extraction, and document
building. They are outside the default `tests/` discovery path, so ordinary
`uv run pytest` does not run benchmarks. Run them explicitly:

```bash
mkdir -p performance/results/benchmark-baseline
uv run --group performance pytest performance/benchmarks -q \
  --benchmark-json=performance/results/benchmark-baseline/benchmark.json \
  --benchmark-save=baseline --benchmark-save-data
```

The plugin calibrates iterations and repeats operations. Assertions check the
final result outside the timed operation. Each extraction creates fresh state;
file reads are included and are generally cached by the OS. Parser creation is
outside timing. Document building uses already extracted entities. No database,
LLM, network calls, or index writes are part of these workloads.

The JSON includes summary statistics and timing samples, machine and commit
information, and custom fixture hash, size, and timed-scope descriptions.
Durations in JSON are seconds even if the console displays microseconds.
The fixture is small: results are a starting baseline, not production throughput.
Run without Scalene, coverage, or parallel pytest workers for comparable timings.

Saved comparison baselines live in `.benchmarks/` (ignored by Git). To compare
with the latest saved run on the same environment:

```bash
uv run --group performance pytest performance/benchmarks -q --benchmark-compare
```

Keep inputs, dependencies, Python, machine, and load comparable. No performance
failure threshold is imposed initially; shared runners introduce timing noise.

Load the exported summary into pandas (separate from the Scalene loader):

```python
import json
from pathlib import Path
import pandas as pd

report = json.loads(
    Path("performance/results/benchmark-baseline/benchmark.json").read_text()
)
benchmarks = pd.json_normalize(report["benchmarks"])
print(benchmarks[["name", "stats.median", "stats.mean", "stats.stddev"]])
```

A manually triggered **Performance benchmarks** GitHub workflow runs these
benchmarks and uploads JSON for later analysis. It does not gate pull requests
on timing thresholds.
