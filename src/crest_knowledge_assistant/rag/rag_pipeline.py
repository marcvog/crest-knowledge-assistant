from crest_knowledge_assistant.indexing.vector_store import VectorStore, SearchHit
from crest_knowledge_assistant.indexing.embedder import Embedder
from crest_knowledge_assistant.indexing.index_store import PROJECT_ROOT
from crest_knowledge_assistant.rag.generator import Generator
import crest_knowledge_assistant.indexing.indexer as indexer

import os
import sys
import time
import uuid
from pathlib import Path
from dotenv import load_dotenv


import streamlit as st

load_dotenv()

def _require_env(name: str) -> str:
    value = os.getenv(name)

    if not value:
        raise RuntimeError(
            f"Missing required environment variable: {name!r}"
        )

    return value

# MILVUS_URI   = _require_env("MILVUS_URI")
# MILVUS_TOKEN = _require_env("MILVUS_KEY")

uri = "db/milvus.db"
token = None

EMBED_MODEL     = indexer.EMBED_MODEL
EMBED_DIM       = indexer.EMBED_DIM
COLLECTION_NAME = indexer.COLLECTION


sys.path.append(PROJECT_ROOT)


class RAGPipeline:
    def __init__(self):
        self.generator = Generator()
        self.embedder = Embedder(EMBED_MODEL, EMBED_DIM)
        self.vector_store = VectorStore(uri, COLLECTION_NAME, EMBED_DIM, token)


    def retrieve(self, question: str, top_k: int = 5) -> list[SearchHit]:
        query_vector = self.embedder.embed_text(question)
        return self.vector_store.search(query_vector, top_k=top_k)


    def generate(self, question: str, hits: list[SearchHit]) -> str:

        context_block = "\n\n".join(
            (
                f"[{i + 1}] "
                f"(Source: {h.metadata['source_file']}, "
                f"lines {h.metadata['start_line']}-{h.metadata['end_line']})\n"
                f"{h.text}"
            )
            for i, h in enumerate(hits)
        )

        system = (
            "You are an engineering assistant answering questions about the "
            "CREST software project. "
            "Use only the retrieved context to answer the user's question. "
            "When you use information from a source, cite it inline like [1] or [2]. "
            "If the retrieved context does not contain enough information to answer "
            "the question, say so rather than guessing."
        )

        user = (
            f"Context:\n{context_block}\n\n"
            f"Question: {question}"
        )

        return self.generator.generate(system, user)
    

    def answer(self, question: str, top_k: int = 5) -> str:
        hits = self.retrieve(question, top_k)
        return self.generate(question,hits)

    
def main() -> None:
    rag = RAGPipeline()

    question = input("Question: ")

    hits = rag.retrieve(question)

    print("\nRetrieved documents:\n")

    for i, hit in enumerate(hits, start=1):
        print(
            f"{i}. {hit.metadata['qualified_name']} "
            f"(score={hit.score:.4f})"
        )

    answer = rag.generate(question, hits)

    print("\nAnswer:\n")
    print(answer)

if __name__ == "__main__":
    main()



