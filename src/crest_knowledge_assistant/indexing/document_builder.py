from crest_knowledge_assistant.models.index_document import IndexDocument
from crest_knowledge_assistant.models.semantic_entity import SemanticEntity, EntityKind
from crest_knowledge_assistant.models.index_version import IndexVersion
from crest_knowledge_assistant.indexing.index_store import PROJECT_ROOT, INDEX_DIR, DOCUMENT_DIR, IndexStore
from crest_knowledge_assistant.file_utils import get_file_paths

from pathlib import Path
import os


class DocumentBuilder:
    def __init__(self):
        self.semantic_entities: list[SemanticEntity] = []
        self.index_documents: list[IndexDocument] = []
        self.store = IndexStore()

    def build(self, entity: SemanticEntity) -> IndexDocument:
        return IndexDocument(
            fragment_id=entity.id,
            entity_id=entity.id,
            text=self.build_text(entity),
            metadata=self.build_metadata(entity),
        )

    def build_text(self, entity: SemanticEntity) -> str:
        parts: list[str] = []

        parts.append(f"Kind: {entity.kind.value}")
        parts.append(f"Qualified name: {entity.qualified_name}")

        if entity.namespace:
            parts.append(f"Namespace: {entity.namespace}")

        parts.append(f"\nSignature:\n{entity.signature}")

        if entity.documentation:
            parts.append(f"\nDocumentation:\n{entity.documentation}")

        parts.append(f"\nSource code:\n{entity.source_code}")

        return "\n".join(parts)


    def build_metadata(self, entity: SemanticEntity) -> dict[str, str]:
        return {
            "kind": entity.kind.value,
            "name": entity.name,
            "qualified_name": entity.qualified_name,
            "namespace": entity.namespace or "",
            "source_file": str(entity.source_file),
            "start_line": str(entity.start_line),
            "end_line": str(entity.end_line),
        }


    def clear (self) -> None:
        self.semantic_entities.clear()
        self.index_documents.clear()


def main():

    builder = DocumentBuilder()
    file_paths = get_file_paths(INDEX_DIR)

    for path in file_paths:
        builder.store.entity_path = path
        builder.semantic_entities = builder.store.load_entities()
        for entity in builder.semantic_entities:
            document = builder.build(entity)
            builder.index_documents.append(document)

        builder.store.document_path = DOCUMENT_DIR / f"{path.stem}_index_documents.json"
        builder.store.save_documents(builder.index_documents)
        builder.clear()


if __name__ == "__main__":
    main()