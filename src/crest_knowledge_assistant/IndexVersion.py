from dataclasses import dataclass
from datetime import datetime


@dataclass
class IndexVersion:
    version_id: str
    git_reference: str
    indexing_date: str
    entity_count: int

    def to_dict (self) -> dict:
        dictionary = {
                "version_id" : self.version_id,
                "git_reference" : self.git_reference,
                "indexing_date" : self.indexing_date,
                "entity_count" : self.entity_count
        }
        return dictionary
