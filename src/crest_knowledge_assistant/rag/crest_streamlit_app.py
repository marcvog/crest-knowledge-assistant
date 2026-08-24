"""Minimal Streamlit chat UI for the CREST Knowledge Assistant.

Run from the project root with:
    streamlit run path/to/crest_streamlit_app.py
"""

import streamlit as st

from crest_knowledge_assistant.rag.rag_pipeline import RAGPipeline


st.set_page_config(
    page_title="CREST Knowledge Assistant",
    page_icon="💬",
)


@st.cache_resource
def get_rag_pipeline() -> RAGPipeline:
    """Create the RAG pipeline once and reuse it across Streamlit reruns."""
    return RAGPipeline()


if "messages" not in st.session_state:
    st.session_state.messages = []


st.title("CREST Knowledge Assistant")
st.caption("Ask questions about the CREST software project.")

with st.sidebar:
    st.header("Settings")
    top_k = st.slider(
        "Retrieved chunks",
        min_value=1,
        max_value=10,
        value=5,
        help="How many relevant code chunks to give to the language model.",
    )


for message in st.session_state.messages:
    with st.chat_message(message["role"]):
        st.markdown(message["content"])


if question := st.chat_input("Ask a question about CREST..."):
    st.session_state.messages.append(
        {"role": "user", "content": question}
    )

    with st.chat_message("user"):
        st.markdown(question)

    with st.chat_message("assistant"):
        with st.spinner("Searching CREST and preparing an answer..."):
            try:
                answer = get_rag_pipeline().answer(question, top_k=top_k)
            except Exception as exc:
                st.error(f"The assistant could not answer: {exc}")
            else:
                st.markdown(answer)
                st.session_state.messages.append(
                    {"role": "assistant", "content": answer}
                )
