from langchain_core.language_models.chat_models import BaseChatModel
from langchain_core.messages import (
    AIMessage,
    HumanMessage,
    SystemMessage,
)
from langchain_core.prompts import ChatPromptTemplate
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
        handler = CallbackHandler()

        langchain_messages = []

        for message in messages:
            role = message["role"]
            content = message["content"]

            if role == "system":
                langchain_messages.append(SystemMessage(content=content))
            elif role == "user":
                langchain_messages.append(HumanMessage(content=content))
            elif role == "assistant":
                langchain_messages.append(AIMessage(content=content))
            else:
                raise ValueError(f"Unsupported message role: {role}")

        if langfuse_prompt is not None:
            prompt_template = ChatPromptTemplate.from_messages(
                [
                    ("system", langfuse_prompt.get_langchain_prompt()),
                    *langchain_messages[1:],
                ]
            )
            prompt_template.metadata = {
                "langfuse_prompt": langfuse_prompt,
            }
        else:
            prompt_template = ChatPromptTemplate.from_messages(langchain_messages)

        chain = prompt_template | self.model

        response = chain.invoke(
            {},
            config={"callbacks": [handler]},
        )

        return str(response.content)
