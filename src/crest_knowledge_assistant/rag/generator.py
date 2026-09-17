from langchain_core.language_models.chat_models import BaseChatModel


class Generator:  
    def __init__(self, model: BaseChatModel):
        self.model = model

    def generate(self, messages: list[dict[str, str]]) -> str:
        response = self.model.invoke(messages)
        return str(response.content)