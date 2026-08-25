

class QueryRouter:

    def route(self, question: str) -> str:
        question = question.lower()

        structural_patterns = [
            "list all methods",
            "list the methods",
            "what methods",
            "find class",
            "list classes",
            "list functions",
        ]

        if any(pattern in question for pattern in structural_patterns):
            return "structural"
        #also return semantic if it fails to find matches

        return "semantic"