from dataclasses import dataclass


@dataclass
class IndexDocument:
    fragment_id: str
    entity_id: str
    text: str
    metadata: dict[str, str]

    def to_dict(self) -> dict:
        dictionary = {
            "fragment_id": self.fragment_id,
            "entity_id": self.entity_id,
            "text": self.text,
            "metadata": self.metadata
        }
        return dictionary

    @classmethod
    def from_dict(cls, data: dict) -> "IndexDocument":
        document = cls(
            fragment_id=data["fragment_id"],
            entity_id=data["entity_id"],
            text=data["text"],
            metadata=data["metadata"]
        )
        return document