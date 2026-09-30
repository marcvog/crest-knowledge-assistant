import pytest

from crest_knowledge_assistant.indexing.embedder import Embedder


@pytest.mark.integration
def test_live_embedding():
    vector = Embedder().embed_text("CrestApi retrieves payloads from the CREST server.")
    assert isinstance(vector, list)
    assert len(vector) > 0
