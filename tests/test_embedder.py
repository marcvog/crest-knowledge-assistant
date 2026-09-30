from unittest.mock import Mock, call

import pytest

from crest_knowledge_assistant.indexing import embedder as module


@pytest.fixture
def backend(monkeypatch):
    for name in ("EMBEDDING_PROVIDER", "EMBEDDING_MODEL", "EMBEDDING_DIMENSIONS"):
        monkeypatch.delenv(name, raising=False)
    monkeypatch.setattr(module, "load_dotenv", lambda: None)
    factory = Mock()
    monkeypatch.setattr(module, "init_embeddings", factory)
    return factory


def test_query_embedding(backend):
    backend.return_value.embed_query.return_value = [0.1, 0.2]
    embedder = module.Embedder(dimensions=2)
    assert embedder.embed_text("question") == [0.1, 0.2]
    backend.assert_called_once_with(
        model="text-embedding-3-small", provider="openai", dimensions=2
    )
    backend.return_value.embed_query.assert_called_once_with("question")
    backend.return_value.embed_documents.assert_not_called()


def test_document_batches_preserve_order(backend):
    backend.return_value.embed_documents.side_effect = [[[1.0], [2.0]], [[3.0]]]
    embedder = module.Embedder()
    assert embedder.embed_texts(["a", "b", "c"], batch_size=2) == [[1.0], [2.0], [3.0]]
    assert backend.return_value.embed_documents.call_args_list == [
        call(["a", "b"]),
        call(["c"]),
    ]


def test_empty_documents_do_not_call_backend(backend):
    assert module.Embedder().embed_texts([]) == []
    backend.return_value.embed_documents.assert_not_called()


@pytest.mark.parametrize("batch_size", [0, -1])
def test_invalid_batch_size(backend, batch_size):
    with pytest.raises(ValueError, match="batch_size must be positive"):
        module.Embedder().embed_texts(["a"], batch_size=batch_size)
    backend.return_value.embed_documents.assert_not_called()


@pytest.mark.parametrize("dimensions", [0, -1])
def test_invalid_dimensions(backend, dimensions):
    with pytest.raises(ValueError, match="dimensions must be positive"):
        module.Embedder(dimensions=dimensions)
    backend.assert_not_called()


def test_environment_and_explicit_settings(backend, monkeypatch):
    monkeypatch.setenv("EMBEDDING_MODEL", "text-embedding-3-large")
    monkeypatch.setenv("EMBEDDING_DIMENSIONS", "8")
    module.Embedder()
    backend.assert_called_with(
        model="text-embedding-3-large", provider="openai", dimensions=8
    )
    module.Embedder(model="text-embedding-3-small", dimensions=2)
    backend.assert_called_with(
        model="text-embedding-3-small", provider="openai", dimensions=2
    )
