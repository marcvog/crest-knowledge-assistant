from crest_knowledge_assistant.indexing.embedder import Embedder
from crest_knowledge_assistant.indexing.vector_store import VectorStore
from crest_knowledge_assistant.indexing.index_store import IndexStore, DOCUMENT_DIR
from crest_knowledge_assistant.file_utils import get_file_paths
from crest_knowledge_assistant.models.index_document import IndexDocument


from pathlib import Path

from dotenv import load_dotenv
import os
import sys
from collections.abc import Callable


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

EMBED_MODEL = "text-embedding-3-small"
EMBED_DIM   = 1536                          # dimension of text-embedding-3-small
COLLECTION  = "crest_knowledge_base_v1"     # Milvus collection name

load_dotenv()
uri = os.getenv("MILVUS_URI")
token = os.getenv("MILVUS_KEY")


class Indexer:
    def __init__(self, embed_model: str, embed_dim: int, collection: str):
        self.embedder = Embedder(embed_model)
        self.index_store = IndexStore()
        self.vector_store = VectorStore(uri,collection,embed_dim,token)


    def build_index(self, on_progress: Callable[[str, int, int], None] | None = None):

        # Load all documents from the index store.
        file_paths: list[Path] = get_file_paths(DOCUMENT_DIR)
        file_paths = [Path(DOCUMENT_DIR / "ChannelSetDto.cxx_index_documents.json")]
        
        for path in file_paths:
            print(f"this file: {path} ")
            self.index_store.document_path=path
            index_documents: list[IndexDocument] = self.index_store.load_documents()
            vectors = self.embedder.embed_texts([document.text for document in index_documents])
            records = [self.vector_store._build_record(doc,vec) for doc, vec in zip(index_documents,vectors)]
            #print(f"records: {records}")
            #self.vector_store.insert(records)


def _cli():
    load_dotenv()
    indexer=Indexer(EMBED_MODEL, EMBED_DIM, COLLECTION)
    print(f"Indexing documents from '{DOCUMENT_DIR}' → collection '{COLLECTION}'...")
    # summary=indexer.build_index(on_progress=lambda step, completed, total:
    #     print(f"  {step}: {completed}/{total}")
    # )
    summary=indexer.build_index()


    print(f"Done: {summary}")
    return 0


if __name__=="__main__":
    sys.exit(_cli())
        