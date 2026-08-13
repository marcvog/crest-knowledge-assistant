from crest_knowledge_assistant.indexing.embedder import Embedder
from crest_knowledge_assistant.models.index_document import IndexDocument
from crest_knowledge_assistant.indexing.index_store import IndexStore, DOCUMENT_DIR
from crest_knowledge_assistant.file_utils import get_file_paths

from pathlib import Path


def main() -> None:

    store: IndexStore = IndexStore()
    file_paths: list[Path] = get_file_paths(DOCUMENT_DIR)
    path: Path = Path(DOCUMENT_DIR / "CrestAuth.h_index_documents.json")
    store.document_path = path
    index_documents: list[IndexDocument] = store.load_documents()

    embedder = Embedder()
    for document in index_documents:
        vector = embedder.embed_text(document.text)
        print(document)
        print(f"Document ID: {document.fragment_id}, Vector Length: {len(vector)}")
        print(f"First 10 elements of the vector: {vector[:10]}")


if __name__ == "__main__":
    main()
