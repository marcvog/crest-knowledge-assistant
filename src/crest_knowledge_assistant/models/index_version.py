from dataclasses import dataclass
from datetime import datetime


@dataclass
class IndexVersion:
    version_id: str
    source_git_reference: str
    application_git_reference: str
    indexing_date: str
    entity_count: int

    def to_dict (self) -> dict:
        dictionary = {
                "version_id" : self.version_id,
                "source_git_reference" : self.source_git_reference,
                "application_git_reference" : self.application_git_reference,
                "indexing_date" : self.indexing_date,
                "entity_count" : self.entity_count
        }
        return dictionary

    @classmethod
    def from_dict (cls, data: dict) -> "IndexVersion":
        version = cls(
            version_id=data["version_id"],
            source_git_reference=data["source_git_reference"],
            application_git_reference=data["application_git_reference"],
            indexing_date=data["indexing_date"],
            entity_count=data["entity_count"]
        )
        return version