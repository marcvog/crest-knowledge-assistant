import re
from dataclasses import dataclass
from enum import Enum


class QueryIntent(Enum):
    LIST_METHODS = "list_methods"
    LIST_CLASSES = "list_classes"
    FIND_METHOD = "find_method"
    FIND_FUNCTION = "find_function"

@dataclass
class StructuralQuery:
    intent: QueryIntent
    target: str | None

@dataclass
class RoutedQuery:
    pipeline: str
    structural_query: StructuralQuery | None = None


class QueryRouter:
    def route(self, question: str) -> RoutedQuery:
        print("I am in the route function:wq")
        structural_query = self.parse_structural_query(question)

        if structural_query:
            return RoutedQuery(
                pipeline="structural",
                structural_query=structural_query,
            )

        return RoutedQuery(
            pipeline="semantic",
        )
    
    def parse_structural_query(self, question: str) -> StructuralQuery | None:
        question = question.strip()

        # Examples:
        # "list all methods of CrestApi"
        # "find all methods in CrestApi"
        # "what methods does CrestApi have?"
        patterns = [
            r"(?:list|find)\s+(?:all\s+)?methods\s+(?:of|in)\s+([\w:]+)",
            r"what\s+methods\s+(?:does|are\s+in)\s+([\w:]+)",
        ]

        for pattern in patterns:
            match = re.search(pattern, question, re.IGNORECASE)

            if match:
                return StructuralQuery(
                    intent=QueryIntent.LIST_METHODS,
                    target=match.group(1),
                )

        # Examples:
        # "list all classes"
        # "find all classes"
        # "list classes"
        if re.search(
            r"(?:list|find)\s+(?:all\s+)?classes\b",
            question,
            re.IGNORECASE,
        ):
            return StructuralQuery(
                intent=QueryIntent.LIST_CLASSES,
                target=None,
            )

        # Examples:
        # "find method getPayload"
        # "find the method getPayload"
        match = re.search(
            r"find\s+(?:the\s+)?method\s+([\w:]+)",
            question,
            re.IGNORECASE,
        )

        if match:
            return StructuralQuery(
                intent=QueryIntent.FIND_METHOD,
                target=match.group(1),
            )

        # Examples:
        # "find function getCrestServerConfig"
        # "find the function getCrestServerConfig"
        match = re.search(
            r"find\s+(?:the\s+)?function\s+([\w:]+)",
            question,
            re.IGNORECASE,
        )

        if match:
            return StructuralQuery(
                intent=QueryIntent.FIND_FUNCTION,
                target=match.group(1),
            )

        return None