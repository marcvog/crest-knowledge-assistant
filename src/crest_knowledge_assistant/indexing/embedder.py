import os

from dotenv import load_dotenv
from langchain.embeddings import init_embeddings
from langchain_core.embeddings import Embeddings


class Embedder:
    def __init__(
        self,
        model: str | None = None,
        dimensions: int | None = None,
        *,
        provider: str | None = None,
    ):
        load_dotenv()

        self.provider = (
            provider if provider is not None else os.getenv("EMBEDDING_PROVIDER", "openai")
        ).strip().lower()
        self.model = model if model is not None else os.getenv("EMBEDDING_MODEL")
        if self.model is None:
            if self.provider == "openai":
                self.model = "text-embedding-3-small"
            else:
                raise ValueError("Set EMBEDDING_MODEL or pass model for this provider.")
        if not self.provider or not self.model.strip():
            raise ValueError("Embedding provider and model must not be empty.")

        if dimensions is None:
            configured_dimensions = os.getenv("EMBEDDING_DIMENSIONS")
            if configured_dimensions is not None:
                dimensions = int(configured_dimensions)
        if dimensions is not None and dimensions <= 0:
            raise ValueError("Embedding dimensions must be positive.")
        self.dimensions = dimensions

        # Output-size configuration is provider/model-specific.
        options = {}
        if self.dimensions is not None:
            if self.provider != "openai" or not self.model.startswith("text-embedding-3-"):
                raise ValueError(
                    "This adapter supports dimensions only for OpenAI text-embedding-3 models. "
                    "Omit EMBEDDING_DIMENSIONS for other providers/models."
                )
            options["dimensions"] = self.dimensions

        self.embeddings: Embeddings = init_embeddings(
            model=self.model,
            provider=self.provider,
            **options,
        )

    def embed_text(self, text: str) -> list[float]:
        """Embed a retrieval query."""
        return self.embeddings.embed_query(text)

    def embed_texts(
        self, texts: list[str], batch_size: int = 64
    ) -> list[list[float]]:
        """Embed documents in input order, using application-level batches."""
        if batch_size <= 0:
            raise ValueError("batch_size must be positive.")

        vectors: list[list[float]] = []
        for start in range(0, len(texts), batch_size):
            vectors.extend(
                self.embeddings.embed_documents(texts[start:start + batch_size])
            )
        return vectors