from crest_knowledge_assistant.indexing.embedder import Embedder
from crest_knowledge_assistant.indexing.vector_store import VectorStore
from crest_knowledge_assistant.indexing.index_store import DOCUMENT_DIR
from crest_knowledge_assistant.file_utils import get_file_paths

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


class Indexer:
    def __init__(self):
        pass
    def build_index(self, on_progress: Callable[[str, int, int], None] | None = None):
        pass


def _cli():
    load_dotenv()
    indexer=Indexer()
    print(f"Indexing documents from '{DOCUMENT_DIR}' → collection '{COLLECTION}'...")
    summary=indexer.build_index(on_progress=lambda step, completed, total:
        print(f"  {step}: {completed}/{total}")
    )
    print(f"Done: {summary}")
    return 0


if __name__=="__main__":
    sys.exit(_cli())
        