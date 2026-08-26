"""Minimal Streamlit chat UI for the CREST Knowledge Assistant.

Run from the project root with:
    streamlit run path/to/crest_streamlit_app.py
"""

import streamlit as st
import re

from crest_knowledge_assistant.rag.rag_pipeline import RAGPipeline
from crest_knowledge_assistant.structural.query_router import QueryRouter
from crest_knowledge_assistant.structural.structural_pipeline import StructPipeline
from dotenv import load_dotenv

import os
import hmac
import streamlit as st

load_dotenv()  # Load environment variables from .env file

def check_password() -> None:
    expected_password = os.getenv("APP_PASSWORD")

    if not expected_password:
        raise RuntimeError("APP_PASSWORD is not configured")

    if "authenticated" not in st.session_state:
        st.session_state.authenticated = False

    if st.session_state.authenticated:
        return

    password = st.text_input(
        "Password",
        type="password",
    )

    if not password:
        st.stop()

    if hmac.compare_digest(password, expected_password):
        st.session_state.authenticated = True
        st.rerun()

    st.error("Incorrect password")
    st.stop()


check_password()

MAX_HISTORY_TURNS = 3

def clean_history_message(text: str) -> str:
    return re.sub(r"\[\d+\]", "", text).strip()

st.set_page_config(
    page_title="CREST Knowledge Assistant",
    page_icon="💬",
)

@st.cache_resource
def get_rag_pipeline() -> RAGPipeline:
    """Create the RAG pipeline once and reuse it across Streamlit reruns."""
    return RAGPipeline()

@st.cache_resource
def get_struct_pipeline() -> StructPipeline:
    """Create the structural pipeline once and reuse it across Streamlit reruns."""
    return StructPipeline()

@st.cache_resource
def get_query_router() -> QueryRouter:
    """Create the query router once and reuse it across Streamlit reruns."""
    return QueryRouter()

if "messages" not in st.session_state:
    st.session_state.messages = []

if "rag_history" not in st.session_state:
    st.session_state.rag_history = []

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
                routed_query = get_query_router().route(question)
                used_rag = False
                if routed_query.pipeline == "structural":
                    answer, hits = get_struct_pipeline().answer(
                        routed_query.structural_query
                        )
                    if not hits:
                        used_rag = True
                        answer, hits = get_rag_pipeline().answer(
                            question,
                            top_k=top_k,
                            history=st.session_state.rag_history[-2 * MAX_HISTORY_TURNS:]
                        )
                else:
                    used_rag = True
                    answer, hits = get_rag_pipeline().answer(
                        question,
                        top_k=top_k,
                        history=st.session_state.rag_history[-2 * MAX_HISTORY_TURNS:]
                    )
            except Exception as exc:
                st.error(f"The assistant could not answer: {exc}")
            else:
                st.markdown(answer)
                st.session_state.last_hits = hits
                st.session_state.messages.append(
                    {"role": "assistant", "content": answer}
                )
                if used_rag:
                    st.session_state.rag_history.extend([
                        {"role": "user", "content": question},
                        {"role": "assistant", "content": clean_history_message(answer)},
                    ])
                st.caption(f"Route: {routed_query.pipeline}")
                with st.expander("Retrieved documents"):
                    for i, hit in enumerate(hits, start=1):
                        st.markdown(
                            f"**{i}. {hit.metadata['qualified_name']}** "
                            f"— similarity `{hit.score:.4f}`"
        )