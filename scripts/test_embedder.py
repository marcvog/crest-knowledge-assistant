from pathlib import Path

from crest_knowledge_assistant.indexing.embedder import Embedder
from crest_knowledge_assistant.indexing.index_store import DOCUMENT_DIR, IndexStore
from crest_knowledge_assistant.models.index_document import IndexDocument


def main() -> None:

    store: IndexStore = IndexStore()
    path: Path = Path(DOCUMENT_DIR / "ChannelSetDto.cxx_index_documents.json")
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
