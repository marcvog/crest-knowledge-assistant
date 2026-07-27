#from indexing import SemanticEntity
from dataclasses import dataclass
from enum import Enum

import os
import json
from pathlib import Path


class EntityKind(Enum):
    NAMESPACE = "namespace"
    CLASS = "class"
    STRUCT = "struct"
    METHOD = "method"
    FUNCTION = "function"
    ENUM = "enum"
    TYPE_ALIAS = "type_alias"

@dataclass
class SemanticEntity:
    kind: EntityKind
    name: str
    qualified_name: str

    namespace: str | None

    source_file: Path
    start_line: int
    end_line: int

    signature: str
    documentation: str | None
    source_code: str

    def to_dict (self) -> dict:
        dictionary = {
                "kind" : self.kind.value,
                "name" : self.name,
                "qualified_name" : self.qualified_name,
                "namespace" : self.namespace,
                "source_file" : str(self.source_file),
                "start_line" : self.start_line,
                "end_line" : self.end_line,
                "signature" : self.signature,
                "documentation" : self.documentation,
                "source_code" : self.source_code
        }
        return dictionary
    
    @classmethod
    def from_dict(cls, data: dict) -> "SemanticEntity":
        entity = cls(
            kind=EntityKind(data["kind"]),
            name=data["name"],
            qualified_name=data["qualified_name"],
            namespace=data["namespace"],
            source_file=Path(data["source_file"]),
            start_line=data["start_line"],
            end_line=data["end_line"],
            signature=data["signature"],
            documentation=data["documentation"],
            source_code=data["source_code"]
        )
        return entity


CURRENT_DIR = os.path.dirname(os.path.realpath(__file__))
path_to_store = os.path.join("entity_store.jsonl")

print(path_to_store)

class EntityStore:

    def save(entities: list[SemanticEntity]) -> None:
        with open(path_to_store, "w", encoding="utf-8") as file:
            for entity in entities:
                json.dump(entity.to_dict(), file)
                file.write("\n")