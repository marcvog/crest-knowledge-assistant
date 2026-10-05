"""Load Scalene 2.x exports into tables without discarding raw profiles."""

import argparse
import json
from pathlib import Path

import pandas as pd

METRICS = {
    "n_cpu_percent_python": "cpu_python_percent",
    "n_cpu_percent_c": "cpu_native_percent",
    "n_sys_percent": "system_percent",
    "n_peak_mb": "peak_mb",
    "n_malloc_mb": "allocated_mb",
    "n_copy_mb_s": "copy_mb_per_second",
}
COLUMNS = ["run_id", "file", "line_number", "source", *METRICS.values()]


def _validate(profile: dict, metadata: dict) -> None:
    if not isinstance(profile, dict) or not isinstance(profile.get("files"), dict):
        raise ValueError("Expected Scalene profile with a files mapping")
    if not isinstance(metadata, dict) or metadata.get("schema_version") != 1:
        raise ValueError("Unsupported run metadata schema")
    if not str(metadata.get("scalene_version", "")).startswith("2."):
        raise ValueError("This loader supports Scalene 2.x profiles")


def load_run(directory: Path) -> tuple[pd.DataFrame, pd.DataFrame]:
    """Return one run row and line measurements; absent values remain missing."""
    profile = json.loads((directory / "profile.json").read_text())
    metadata = json.loads((directory / "metadata.json").read_text())
    _validate(profile, metadata)
    run_id = directory.name
    rows = []
    for filename, detail in profile["files"].items():
        if not isinstance(detail, dict) or not isinstance(
            detail.get("lines", []), list
        ):
            raise ValueError(f"Invalid line data for {filename}")
        for line in detail.get("lines", []):
            if not isinstance(line, dict) or "lineno" not in line:
                raise ValueError(f"Missing line number in {filename}")
            row = {
                "run_id": run_id,
                "file": filename,
                "line_number": line["lineno"],
                "source": line.get("line"),
            }
            row.update({column: line.get(key) for key, column in METRICS.items()})
            rows.append(row)
    lines = pd.DataFrame(rows, columns=COLUMNS)
    for column in METRICS.values():
        lines[column] = pd.to_numeric(lines[column], errors="raise").astype("Float64")
    if not profile.get("memory", False):
        for column in ("peak_mb", "allocated_mb", "copy_mb_per_second"):
            lines[column] = pd.NA
    runs = pd.DataFrame(
        [
            {
                **metadata,
                "run_id": run_id,
                "profile_seconds": profile.get("elapsed_time_sec"),
                "profile_peak_mb": profile.get("max_footprint_mb")
                if profile.get("memory")
                else None,
            }
        ]
    )
    return runs, lines


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Export Scalene tables and a CPU hotspot chart"
    )
    parser.add_argument("directory", type=Path)
    args = parser.parse_args()
    runs, lines = load_run(args.directory)
    runs.to_csv(args.directory / "runs.csv", index=False)
    lines.to_csv(args.directory / "lines.csv", index=False)
    ranked = lines.assign(
        cpu_percent=lines[["cpu_python_percent", "cpu_native_percent"]].sum(
            axis=1, min_count=2
        )
    )
    ranked = ranked.dropna(subset=["cpu_percent"]).nlargest(15, "cpu_percent")
    print(
        ranked[["file", "line_number", "cpu_percent", "peak_mb"]].to_string(index=False)
    )
    if ranked.empty:
        print("No CPU samples available; try a longer workload.")
        return
    import matplotlib

    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    labels = [f"{Path(row.file).name}:{row.line_number}" for row in ranked.itertuples()]
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.barh(labels[::-1], ranked["cpu_percent"].iloc[::-1].astype(float))
    ax.set_xlabel("Python + native CPU attribution (%)")
    fig.tight_layout()
    fig.savefig(args.directory / "cpu-hotspots.png")
    plt.close(fig)


if __name__ == "__main__":
    main()
