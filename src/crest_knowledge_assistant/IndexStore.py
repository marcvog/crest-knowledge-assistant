from SemanticEntity import SemanticEntity
from IndexVersion import IndexVersion

import os
import json
from pathlib import Path

path = Path(__file__).resolve()
while not (path / "pyproject.toml").exists():
    path = path.parent

PROJECT_ROOT = path
INDEX_DIR = PROJECT_ROOT / "index"

INDEX_STORE = INDEX_DIR / "index_version.json"
ENTITY_STORE = INDEX_DIR / "entity_store.jsonl"

class IndexStore:
    def __init__(self, index_store_path: Path = INDEX_STORE, entity_store_path: Path = ENTITY_STORE):
        self.index_store_path = index_store_path
        self.entity_store_path = entity_store_path

    def save_index(self, version: IndexVersion) -> None:
        with self.index_store_path.open("w", encoding="utf-8") as file:
                json.dump(version.to_dict(), file)
                file.write("\n")

    def save_entities(self, entities: list[SemanticEntity]) -> None:
        with self.entity_store_path.open("w", encoding="utf-8") as file:
            for entity in entities:
                json.dump(entity.to_dict(), file)
                file.write("\n")