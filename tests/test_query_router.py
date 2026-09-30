import pytest

from crest_knowledge_assistant.structural.query_router import QueryIntent, QueryRouter


@pytest.mark.parametrize(
    ("question", "intent", "target"),
    [
        ("  LIST ALL METHODS OF CrestApi  ", QueryIntent.LIST_METHODS, "CrestApi"),
        (
            "what methods does crest::Client have?",
            QueryIntent.LIST_METHODS,
            "crest::Client",
        ),
        ("find all classes", QueryIntent.LIST_CLASSES, None),
        ("find the method getPayload", QueryIntent.FIND_METHOD, "getPayload"),
        (
            "find function crest::configure",
            QueryIntent.FIND_FUNCTION,
            "crest::configure",
        ),
    ],
)
def test_structural_routes(question, intent, target):
    result = QueryRouter().route(question)
    assert result.pipeline == "structural"
    assert result.structural_query is not None
    assert result.structural_query.intent == intent
    assert result.structural_query.target == target


@pytest.mark.parametrize("question", ["", "How does payload caching work?"])
def test_semantic_fallback(question):
    result = QueryRouter().route(question)
    assert result.pipeline == "semantic"
    assert result.structural_query is None
