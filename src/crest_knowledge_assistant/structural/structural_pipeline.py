from crest_knowledge_assistant.indexing.vector_store import SearchHit
from crest_knowledge_assistant.models.index_document import IndexDocument
from crest_knowledge_assistant.indexing.index_store import IndexStore, DOCUMENT_DIR
from crest_knowledge_assistant.structural.query_router import QueryIntent, StructuralQuery
from crest_knowledge_assistant.file_utils import get_file_paths

class StructPipeline:
    def __init__(self):
        self.index_store = IndexStore()

    def answer(self, structural_query: StructuralQuery) -> tuple[str, list[SearchHit]]:

        index_documents: list[IndexDocument] = []
        for path in get_file_paths(DOCUMENT_DIR):
            self.index_store.document_path=path
            index_documents.extend(self.index_store.load_documents())
        print(index_documents)

        if structural_query.intent == QueryIntent.LIST_METHODS:
            hits = self._list_methods(
                index_documents,
                structural_query.target,
            )

        elif structural_query.intent == QueryIntent.LIST_CLASSES:
            hits = self._list_classes(index_documents)

        elif structural_query.intent == QueryIntent.FIND_METHOD:
            hits = self._find_entity(
                index_documents,
                kind="method",
                target=structural_query.target,
            )

        elif structural_query.intent == QueryIntent.FIND_FUNCTION:
            hits = self._find_entity(
                index_documents,
                kind="function",
                target=structural_query.target,
            )

        else:
            return "", []

        if not hits:
            return "", []

        answer = self._build_answer(
            structural_query,
            hits,
        )

        return answer, hits


    def _list_methods(
        self,
        documents: list[IndexDocument],
        class_name: str | None,
    ) -> list[SearchHit]:

        if class_name is None:
            return []

        hits: list[SearchHit] = []

        for document in documents:
            metadata = document.metadata

            if metadata["kind"] != "method":
                continue

            qualified_name = str(metadata["qualified_name"])

            if f"::{class_name}::" not in qualified_name:
                continue

            hits.append(self._to_search_hit(document))

        return hits


    def _list_classes(
        self,
        documents: list[IndexDocument],
    ) -> list[SearchHit]:

        return [
            self._to_search_hit(document)
            for document in documents
            if document.metadata["kind"] == "class"
        ]


    def _find_entity(
        self,
        documents: list[IndexDocument],
        kind: str,
        target: str | None,
    ) -> list[SearchHit]:

        if target is None:
            return []

        return [
            self._to_search_hit(document)
            for document in documents
            if document.metadata["kind"] == kind
            and document.metadata["name"] == target
        ]


    def _to_search_hit(
        self,
        document: IndexDocument,
    ) -> SearchHit:

        return SearchHit(
            fragment_id=document.fragment_id,
            entity_id=document.entity_id,
            text=document.text,
            score=1.0,
            metadata=document.metadata,
        )


    def _build_answer(
        self,
        structural_query: StructuralQuery,
        hits: list[SearchHit],
    ) -> str:

        if structural_query.intent == QueryIntent.LIST_METHODS:
            lines = [
                f"Methods found in {structural_query.target}:"
            ]

        elif structural_query.intent == QueryIntent.LIST_CLASSES:
            lines = ["Classes found:"]

        elif structural_query.intent == QueryIntent.FIND_METHOD:
            lines = [
                f"Method '{structural_query.target}' found:"
            ]

        elif structural_query.intent == QueryIntent.FIND_FUNCTION:
            lines = [
                f"Function '{structural_query.target}' found:"
            ]

        else:
            return ""

        for index, hit in enumerate(hits, start=1):
            qualified_name = hit.metadata["qualified_name"]
            source_file = hit.metadata["source_file"]
            start_line = hit.metadata["start_line"]
            end_line = hit.metadata["end_line"]

            lines.append(
                f"{index}. {qualified_name} "
                f"({source_file}, lines {start_line}-{end_line})"
            )

        return "\n".join(lines)