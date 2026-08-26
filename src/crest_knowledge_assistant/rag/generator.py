from openai import OpenAI


class Generator:
    def __init__(self, model: str = "gpt-5-mini", temperature: float = 1.0):
        self.client = OpenAI()
        self.model = model
        self.temperature = temperature

    def generate(
        self,
        messages: list[dict[str, str]],
    ) -> str:
        response = self.client.chat.completions.create(
            model=self.model,
            temperature=self.temperature,
            messages=messages,
        )

        return response.choices[0].message.content or ""