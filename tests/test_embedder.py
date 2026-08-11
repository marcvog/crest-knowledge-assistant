from crest_knowledge_assistant.indexing.embedder import Embedder


def test_embed_text() -> None:
    embedder = Embedder()

    vector = embedder.embed_text(
        "CrestApi retrieves payloads from the CREST server."
    )

    assert isinstance(vector, list)
    assert len(vector) > 0