"""Repeatable extraction workload; run in a dedicated profiling process."""

import argparse
import hashlib
import json
import os
import platform
import sys
from contextlib import redirect_stdout
from datetime import UTC, datetime
from importlib.metadata import version
from pathlib import Path
from time import perf_counter

import tree_sitter_cpp
from git import Repo
from tree_sitter import Language, Parser

from crest_knowledge_assistant.extraction import semantic_entity_extractor as extraction


def run(input_dir: Path, output_dir: Path, repetitions: int) -> dict:
    if repetitions < 1:
        raise ValueError("repetitions must be positive")
    input_dir = input_dir.resolve()
    files = sorted(
        p
        for p in input_dir.rglob("*")
        if p.suffix in {".h", ".cxx", ".cpp"} and p.is_file() and ".git" not in p.parts
    )
    if not files:
        raise ValueError("No C++ input files found")
    output_dir.mkdir(parents=True, exist_ok=True)
    digest = hashlib.sha256()
    for path in files:
        digest.update(path.relative_to(input_dir).as_posix().encode() + b"\0")
        digest.update(hashlib.sha256(path.read_bytes()).digest())
    repo = Repo(Path(__file__).resolve(), search_parent_directories=True)
    metadata = {
        "schema_version": 1,
        "started_at": datetime.now(UTC).isoformat(),
        "commit": repo.head.commit.hexsha,
        "dirty": repo.is_dirty(untracked_files=True),
        "dataset_sha256": digest.hexdigest(),
        "input_files": len(files),
        "input_bytes": sum(p.stat().st_size for p in files),
        "repetitions": repetitions,
        "python": sys.version,
        "platform": platform.platform(),
        "machine": platform.machine(),
        "scalene_version": version("scalene"),
        "logging": "stdout redirected to devnull; print calls still execute",
    }
    old_data, old_root = extraction.DATA_DIR, extraction.PROJECT_ROOT
    parser = Parser(Language(tree_sitter_cpp.language()))
    entity_count = 0
    started = perf_counter()
    try:
        extraction.DATA_DIR = input_dir
        extraction.PROJECT_ROOT = input_dir
        with open(os.devnull, "w") as sink, redirect_stdout(sink):
            for _ in range(repetitions):
                for path in files:
                    tree = parser.parse(path.read_bytes())
                    if tree.root_node.has_error:
                        raise ValueError(f"Invalid C++ fixture: {path}")
                    extractor = extraction.EntityExtractor(path)
                    extractor.walk(tree.root_node)
                    entity_count += len(extractor.entities)
    finally:
        extraction.DATA_DIR, extraction.PROJECT_ROOT = old_data, old_root
    metadata["workload_seconds"] = perf_counter() - started
    metadata["entities_processed"] = entity_count
    (output_dir / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
    return metadata


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input-dir", type=Path, default=Path("performance/fixtures"))
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--repetitions", type=int, default=1000)
    args = parser.parse_args()
    run(args.input_dir, args.output_dir, args.repetitions)


if __name__ == "__main__":
    main()
