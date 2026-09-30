from pathlib import Path

import tree_sitter_cpp
from tree_sitter import Language, Parser

from crest_knowledge_assistant.extraction import semantic_entity_extractor as extraction
from crest_knowledge_assistant.indexing.document_builder import DocumentBuilder
from crest_knowledge_assistant.indexing.index_store import IndexStore
from crest_knowledge_assistant.models.semantic_entity import EntityKind, SemanticEntity


def test_extract_cpp_scopes(tmp_path, monkeypatch):
    monkeypatch.setattr(extraction, "DATA_DIR", tmp_path)
    monkeypatch.setattr(extraction, "PROJECT_ROOT", tmp_path)
    source = tmp_path / "sample.cxx"
    source.write_text(
        "namespace crest { class Client { public: int fetch() { return 1; } }; int count() { return 2; } }"
    )
    tree = Parser(Language(tree_sitter_cpp.language())).parse(source.read_bytes())
    extractor = extraction.EntityExtractor(source)
    extractor.walk(tree.root_node)
    entities = {entity.qualified_name: entity for entity in extractor.entities}
    assert entities["crest::Client"].kind == EntityKind.CLASS
    assert entities["crest::Client::fetch"].kind == EntityKind.METHOD
    assert entities["crest::count"].kind == EntityKind.FUNCTION
    assert entities["crest::count"].namespace == "crest"
    assert entities["crest::count"].source_file == Path("sample.cxx")
    assert not extractor.namespace_stack
    assert not extractor.class_stack
    assert not extractor.struct_stack


def test_document_metadata_and_storage(tmp_path):
    entity = SemanticEntity(
        id="constant-id",
        kind=EntityKind.CONSTANT,
        name="limit",
        qualified_name="crest::limit",
        namespace="crest",
        source_file=Path("data/sample.h"),
        start_line=3,
        end_line=3,
        signature="constexpr int limit",
        documentation="Maximum items.",
        source_code="constexpr int limit = 4;",
        constant_value="4",
    )
    document = DocumentBuilder().build(entity)
    assert document.entity_id == entity.id
    assert document.fragment_id == entity.id
    assert document.metadata["start_line"] == 3
    assert document.metadata["source_file"] == "data/sample.h"
    assert "Value: 4" in document.text
    assert "Documentation:\nMaximum items." in document.text
    assert "Qualified name: crest::limit" in document.text
    store = IndexStore(
        entity_path=tmp_path / "entities.jsonl",
        document_path=tmp_path / "documents.jsonl",
    )
    store.save_entities([entity])
    store.save_documents([document])
    assert store.load_entities() == [entity]
    assert store.load_documents() == [document]
