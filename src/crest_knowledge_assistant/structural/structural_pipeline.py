from crest_knowledge_assistant.indexing.vector_store import VectorStore, SearchHit

class StructPipeline:

    def answer(self, question: str, top_k: int) -> tuple[str,list[SearchHit]]:
        pass