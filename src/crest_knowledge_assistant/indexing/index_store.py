from crest_knowledge_assistant.models.semantic_entity import SemanticEntity
from crest_knowledge_assistant.models.index_version import IndexVersion
from crest_knowledge_assistant.models.index_document import IndexDocument

import os
import json
from pathlib import Path

path = Path(__file__).resolve()
while not (path / "pyproject.toml").exists():
    path = path.parent

PROJECT_ROOT = path

VERSION_DIR = PROJECT_ROOT / "version"
INDEX_DIR = PROJECT_ROOT / "index"
DOCUMENT_DIR = PROJECT_ROOT / "documents"

VERSION_STORE = VERSION_DIR / "index_versions.json"
ENTITY_STORE = INDEX_DIR / "semantic_entities.jsonl"
DOCUMENT_STORE = DOCUMENT_DIR / "index_documents.json"

class IndexStore:
    def __init__(self, entity_path: Path = ENTITY_STORE, document_path: Path = DOCUMENT_STORE):
        self.entity_path = entity_path
        self.document_path = document_path

    def save_version(self, version: IndexVersion) -> None:
        with VERSION_STORE.open("w", encoding="utf-8") as file:
            json.dump(version.to_dict(), file)
            file.write("\n")

    def save_entities(self, entities: list[SemanticEntity]) -> None:
        with self.entity_path.open("w", encoding="utf-8") as file:
            for entity in entities:
                json.dump(entity.to_dict(), file)
                file.write("\n")

    def load_entities(self) -> list[SemanticEntity]:
        entities = []
        with self.entity_path.open("r", encoding="utf-8") as file:
            for line in file:
                data = json.loads(line)
                entity = SemanticEntity.from_dict(data)
                entities.append(entity)
        return entities

    def save_documents(self, documents: list[IndexDocument]) -> None:
        with self.document_path.open("w", encoding="utf-8") as file:
            for document in documents:
                json.dump(document.to_dict(), file)
                file.write("\n")

    def load_documents(self) -> list[IndexDocument]:
        documents = []
        with self.document_path.open("r", encoding="utf-8") as file:
            for line in file:
                data = json.loads(line)
                document = IndexDocument.from_dict(data)
                documents.append(document)
        return documents