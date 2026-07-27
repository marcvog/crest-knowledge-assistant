from SemanticEntity import SemanticEntity

import os
import json
from pathlib import Path

CURRENT_DIR: str = os.path.dirname(os.path.realpath(__file__))
path_to_store: str = os.path.join(CURRENT_DIR, "entity_store.jsonl")

class EntityStore:
    def __init__(self, path: str = path_to_store):
        self.path = path

    def save(self, entities: list[SemanticEntity]) -> None:
        with open(self.path, "w", encoding="utf-8") as file:
            for entity in entities:
                json.dump(entity.to_dict(), file)
                file.write("\n")