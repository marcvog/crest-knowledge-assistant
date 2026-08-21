from openai import OpenAI
from dotenv import load_dotenv


class Embedder:
    def __init__(self, model: str = "text-embedding-3-small", dimensions: int = 1536):
        self.client = OpenAI()
        self.model = model
        self.dimensions = dimensions


    def embed_text(self, text: str) -> list[float]:
        load_dotenv()

        response = self.client.embeddings.create(
            model=self.model,
            input=text,
            dimensions=self.dimensions
        )

        return response.data[0].embedding


    def embed_texts(self, texts: list[str], batch_size: int = 64,) -> list[list[float]]:
        load_dotenv()
        
        vectors: list[list[float]] = []

        for start in range(0, len(texts), batch_size):
            batch = texts[start:start + batch_size]

            response = self.client.embeddings.create(
                model=self.model,
                input=batch,
                dimensions=self.dimensions
            )

            vectors.extend(
                item.embedding
                for item in response.data
            )

        return vectors