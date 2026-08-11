from crest_knowledge_assistant.indexing.embedder import Embedder


def main() -> None:
    embedder = Embedder()

    vector = embedder.embed_text(
        "CrestApi retrieves payloads from the CREST server."
    )

    print(type(vector))
    print(len(vector))
    print(vector[:10])


if __name__ == "__main__":
    main()
