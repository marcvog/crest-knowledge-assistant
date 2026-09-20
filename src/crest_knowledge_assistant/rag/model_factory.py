from langchain.chat_models import init_chat_model
from langchain_core.language_models.chat_models import BaseChatModel


def create_chat_model(
    provider: str,
    model: str,
) -> BaseChatModel:
    return init_chat_model(
        model=model,
        model_provider=provider,
    )
