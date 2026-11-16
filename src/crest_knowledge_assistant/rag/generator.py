from langchain_core.language_models.chat_models import BaseChatModel
from langfuse.langchain import CallbackHandler
from langfuse.model import TextPromptClient


class Generator:
    def __init__(self, model: BaseChatModel):
        self.model = model

    def generate(
        self,
        messages: list[dict[str, str]],
        langfuse_prompt: TextPromptClient | None = None,
    ) -> str:
        langfuse_handler = CallbackHandler()

        config = {"callbacks": [langfuse_handler]}

        if langfuse_prompt:
            config["metadata"] = {"langfuse_prompt": langfuse_prompt}

        response = self.model.invoke(
            messages,
            config=config,
        )
        return str(response.content)
