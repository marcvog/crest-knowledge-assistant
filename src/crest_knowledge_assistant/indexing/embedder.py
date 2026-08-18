from openai import OpenAI
from dotenv import load_dotenv


class Embedder(OpenAI):
    def __init__(self, model: str = "text-embedding-3-small", **kwargs):
        super().__init__(**kwargs)
        self.model = model

    def embed_text(self, text: str) -> list[float]:
        load_dotenv()
        response = self.embeddings.create(
            model=self.model,
            input=text,
        )

        return response.data[0].embedding