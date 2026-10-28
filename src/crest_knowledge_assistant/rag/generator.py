from langchain_core.language_models.chat_models import BaseChatModel
from langfuse.langchain import CallbackHandler


class Generator:
    def __init__(self, model: BaseChatModel):
        self.model = model

    def generate(self, messages: list[dict[str, str]]) -> str:
        langfuse_handler = CallbackHandler()

        response = self.model.invoke(
            messages,
            config={"callbacks": [langfuse_handler]},
        )
        return str(response.content)
