from dataclasses import dataclass
from enum import Enum
from pathlib import Path


class EntityKind(Enum):
    NAMESPACE = "namespace"
    CLASS = "class"
    STRUCT = "struct"
    METHOD = "method"
    FUNCTION = "function"
    ENUM = "enum"
    TYPE_ALIAS = "type_alias"
    TEST = "test"
    TEST_INSTANTIATION = "test_instantiation"
    CONSTANT = "constant"


@dataclass
class SemanticEntity:
    id: str
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
    test_macro: str | None = None
    test_suite: str | None = None
    test_parameter_expression: str | None = None
    constant_value: str | None = None

    def to_dict(self) -> dict:
        dictionary = {
            "id": self.id,
            "kind": self.kind.value,
            "name": self.name,
            "qualified_name": self.qualified_name,
            "namespace": self.namespace,
            "source_file": str(self.source_file),
            "start_line": self.start_line,
            "end_line": self.end_line,
            "signature": self.signature,
            "documentation": self.documentation,
            "source_code": self.source_code,
            "test_macro": self.test_macro,
            "test_suite": self.test_suite,
            "test_parameter_expression": self.test_parameter_expression,
            "constant_value": self.constant_value,
        }
        return dictionary

    @classmethod
    def from_dict(cls, data: dict) -> "SemanticEntity":
        entity = cls(
            id=data["id"],
            kind=EntityKind(data["kind"]),
            name=data["name"],
            qualified_name=data["qualified_name"],
            namespace=data["namespace"],
            source_file=Path(data["source_file"]),
            start_line=data["start_line"],
            end_line=data["end_line"],
            signature=data["signature"],
            documentation=data["documentation"],
            source_code=data["source_code"],
            test_macro=data["test_macro"],
            test_suite=data["test_suite"],
            test_parameter_expression=data["test_parameter_expression"],
            constant_value=data["constant_value"],
        )
        return entity
