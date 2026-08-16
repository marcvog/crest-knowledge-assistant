from pymilvus import MilvusClient, DataType
from typing import Any
from dataclasses import dataclass

from crest_knowledge_assistant.models.index_document import IndexDocument


@dataclass
class SearchHit:
    fragment_id: str
    entity_id: str
    text: str
    score: float
    metadata: dict[str, str | int]


class VectorStore:
    def __init__(
        self,
        uri: str,
        collection_name: str,
        vector_dimension: int,
        token: str | None = None,
    ):
        if token:
            self.client = MilvusClient(
                uri=uri,
                token=token,
            )
        else:
            self.client = MilvusClient(
                uri=uri,
            )

        self.collection_name = collection_name
        self.vector_dimension = vector_dimension

        self._ensure_collection()


    def _ensure_collection(self) -> None:

        if self.client.has_collection(self.collection_name):
            self.client.load_collection(self.collection_name)
            return

        schema = MilvusClient.create_schema(
            auto_id=False,
            enable_dynamic_field=False,
        )

        schema.add_field(
            field_name="fragment_id",
            datatype=DataType.VARCHAR,
            is_primary=True,
            max_length=64,
        )

        schema.add_field(
            field_name="entity_id",
            datatype=DataType.VARCHAR,
            max_length=64,
        )

        schema.add_field(
            field_name="vector",
            datatype=DataType.FLOAT_VECTOR,
            dim=1536,
        )

        schema.add_field(
            field_name="text",
            datatype=DataType.VARCHAR,
            max_length=65535,
        )

        schema.add_field(
            field_name="kind",
            datatype=DataType.VARCHAR,
            max_length=32,
        )

        schema.add_field(
            field_name="name",
            datatype=DataType.VARCHAR,
            max_length=512,
        )

        schema.add_field(
            field_name="qualified_name",
            datatype=DataType.VARCHAR,
            max_length=2048,
        )

        schema.add_field(
            field_name="namespace",
            datatype=DataType.VARCHAR,
            max_length=1024,
        )

        schema.add_field(
            field_name="source_file",
            datatype=DataType.VARCHAR,
            max_length=2048,
        )

        schema.add_field(
            field_name="start_line",
            datatype=DataType.INT64,
        )

        schema.add_field(
            field_name="end_line",
            datatype=DataType.INT64,
        )

        index_params = MilvusClient.prepare_index_params()
        index_params.add_index(
            field_name="vector",
            index_type="AUTOINDEX",
            metric_type="COSINE",
        )

        self.client.create_collection(
            collection_name=self.collection_name,
            schema=schema,
            index_params=index_params,
        )

        self.client.load_collection(self.collection_name)
        return


    def reset_collection(self) -> None:
        """Delete all indexed vectors by dropping and recreating the collection.

        Warning:
            This operation is destructive. Existing vectors cannot be recovered
            unless they are regenerated from the index documents.
        """
        if self.client.has_collection(self.collection_name):
            self.client.drop_collection(self.collection_name)
        self._ensure_collection()


    def collection_stats(self) -> dict[str, int]:
        if not self.client.has_collection(self.collection_name):
            return {"row_count": 0}

        rows = self.client.query(
            collection_name=self.collection_name,
            filter="",
            output_fields=["count(*)"],
        )

        if rows:
            return {"row_count": int(rows[0].get("count(*)", 0))}

        return {"row_count": 0}


    def _build_record(
            self,
            document: IndexDocument,
            vector: list[float],
        ) -> dict[str, Any]:

        return {
            "fragment_id": document.fragment_id,
            "entity_id": document.entity_id,
            "vector": vector,
            "text": document.text,
            **document.metadata,
        }


    def insert(self, records: list[dict[str, Any]]) -> int:
        if not records:
            return 0

        self.client.insert(
            collection_name=self.collection_name,
            data=records,
        )

        return len(records)


    def flush(self) -> None:
        self.client.flush(
            collection_name=self.collection_name,
        )


    def search(
        self,
        query_vector: list[float],
        top_k: int = 3,
    ) -> list[SearchHit]:

        raw = self.client.search(
            collection_name=self.collection_name,
            data=[query_vector],
            limit=top_k,
            output_fields=[
                "fragment_id",
                "entity_id",
                "text",
                "kind",
                "name",
                "qualified_name",
                "namespace",
                "source_file",
                "start_line",
                "end_line",
            ],
            search_params={
                "metric_type": "COSINE",
            },
            anns_field="vector",
        )

        hits: list[SearchHit] = []

        for hit in raw[0]:
            entity = hit["entity"]

            hits.append(
                SearchHit(
                    fragment_id=entity["fragment_id"],
                    entity_id=entity["entity_id"],
                    text=entity["text"],
                    score=float(hit["distance"]),
                    metadata={
                        "kind": entity["kind"],
                        "name": entity["name"],
                        "qualified_name": entity["qualified_name"],
                        "namespace": entity["namespace"],
                        "source_file": entity["source_file"],
                        "start_line": entity["start_line"],
                        "end_line": entity["end_line"],
                    },
                )
            )

        return hits   

    
        