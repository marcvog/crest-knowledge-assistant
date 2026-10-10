"""Explicit offline benchmarks; excluded from default pytest testpaths."""

import hashlib
import os
from contextlib import redirect_stdout
from pathlib import Path

import pytest
import tree_sitter_cpp
from tree_sitter import Language, Parser

from crest_knowledge_assistant.extraction import semantic_entity_extractor as extraction
from crest_knowledge_assistant.indexing.document_builder import DocumentBuilder

FIXTURE = Path(__file__).resolve().parents[1] / "fixtures" / "sample.cxx"
EXPECTED = {"crest::Client", "crest::Client::fetch", "crest::count", "crest::limit"}


@pytest.fixture
def cpp(benchmark, monkeypatch):
    source = FIXTURE.read_bytes()
    benchmark.extra_info.update(
        {
            "dataset_sha256": hashlib.sha256(source).hexdigest(),
            "input_bytes": len(source),
            "fixture": "performance/fixtures/sample.cxx",
            "logging": "stdout to devnull; print calls included",
        }
    )
    monkeypatch.setattr(extraction, "DATA_DIR", FIXTURE.parent)
    monkeypatch.setattr(extraction, "PROJECT_ROOT", FIXTURE.parent)
    return Parser(Language(tree_sitter_cpp.language())), source


def extract(parser):
    tree = parser.parse(FIXTURE.read_bytes())
    extractor = extraction.EntityExtractor(FIXTURE)
    extractor.walk(tree.root_node)
    return extractor.entities


def test_parse_cpp(benchmark, cpp):
    parser, source = cpp
    benchmark.extra_info["timed_scope"] = (
        "parse preloaded bytes; excludes file I/O and parser creation"
    )
    tree = benchmark(parser.parse, source)
    assert not tree.root_node.has_error
    assert tree.root_node.type == "translation_unit"


def test_extract_cpp(benchmark, cpp):
    parser, _ = cpp
    benchmark.extra_info["timed_scope"] = (
        "file read, parsing, fresh extractor and traversal; excludes parser creation"
    )
    with open(os.devnull, "w") as sink, redirect_stdout(sink):
        entities = benchmark(extract, parser)
    assert {entity.qualified_name for entity in entities} == EXPECTED


def test_build_documents(benchmark, cpp):
    parser, _ = cpp
    with open(os.devnull, "w") as sink, redirect_stdout(sink):
        entities = extract(parser)
    builder = DocumentBuilder()
    benchmark.extra_info["timed_scope"] = (
        "build documents from pre-extracted entities; excludes extraction and storage"
    )
    benchmark.extra_info["entity_count"] = len(entities)

    def build():
        return [builder.build(entity) for entity in entities]

    documents = benchmark(build)
    assert {document.metadata["qualified_name"] for document in documents} == EXPECTED
    assert all(document.text for document in documents)
