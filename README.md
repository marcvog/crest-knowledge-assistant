# CREST Knowledge Assistant

An AI-powered engineering assistant combining Retrieval-Augmented Generation (RAG), technical documentation retrieval, and source code understanding for scientific software projects.

## Overview

The CREST Knowledge Assistant is designed to help developers and users navigate the CERN ATLAS CREST ecosystem by providing intelligent access to documentation and source code through Large Language Models (LLMs).

The project integrates Retrieval-Augmented Generation (RAG), semantic search, and source code indexing to support:

* Technical documentation search
* Source code exploration and understanding
* AI-assisted software development
* Engineering knowledge retrieval

Although initially developed around the public ATLAS CREST repositories, the architecture is intended to be reusable for other scientific software projects.

## Architecture

The system consists of an offline indexing and ingestion pipeline and an online query and retrieval pipeline:

![CREST Knowledge Assistant architecture](docs/architecture.png)

## Objectives

* Build a scalable RAG pipeline for technical documentation.
* Index source code repositories for semantic retrieval.
* Develop an AI assistant capable of answering engineering questions with citations.
* Evaluate retrieval quality and LLM performance using reproducible benchmarks.
* Apply modern LLMOps and software engineering practices.

## Technology Stack

* Python
* FastAPI
* Retrieval-Augmented Generation (RAG)
* Vector database
* Large Language Models (LLMs)
* Docker
* GitHub Actions

## Python code quality

Install development dependencies with `uv sync --locked`. Run the same Ruff
checks used by GitHub Actions before submitting changes:

```bash
uv run ruff check .
uv run ruff format --check .
```

To apply safe lint fixes (including import sorting) and format the code:

```bash
uv run ruff check --fix .
uv run ruff format .
```

Ruff targets Python 3.12 and checks project code, excluding external source data
and generated documents, indexes, and version metadata. Configuration lives in
`pyproject.toml`; the tool version is recorded in `uv.lock`. The Ruff workflow
runs on pull requests and pushes to `main`.

For feedback while editing in VS Code, install the **Ruff** extension
(`charliermarsh.ruff`) and select Ruff as the Python formatter. Enable format on
save if desired.

## Setup and Usage

Run the following commands from the project root in the order shown.

### Offline indexing and ingestion pipeline

The first three steps prepare the searchable knowledge base. Run them whenever the source data needs to be extracted and reindexed.

1. Extract semantic entities:

   ```bash
   uv run python src/crest_knowledge_assistant/extraction/semantic_entity_extractor.py
   ```

2. Create index documents:

   ```bash
   uv run python src/crest_knowledge_assistant/indexing/document_builder.py
   ```

3. Insert embeddings (vectors) into the vector database:

   ```bash
   uv run python src/crest_knowledge_assistant/indexing/indexer.py
   ```

### Online chat interface

After the offline pipeline has completed, start the Streamlit chat interface:

```bash
uv run streamlit run src/crest_knowledge_assistant/rag/crest_streamlit_app.py
```


## Type checking and tests

Install the locked development and application dependencies with `uv sync --locked`.

```bash
uv run mypy
uv run pytest
```

Mypy initially checks models, file utilities, query routing, document building,
index storage, and embeddings. Its scope is listed in `[tool.mypy].files` in
`pyproject.toml`; extraction, vector storage, and the UI/RAG pipelines are not yet
fully type-checked. Functions in the selected modules must have annotations,
and untyped function bodies are checked. This is an incremental baseline, not
strict checking of the entire application.

The default tests run offline, with embedding clients replaced by test doubles
and extraction/storage fixtures in temporary directories. They do not require
the CrestApi submodule, API credentials, or a running database. GitHub Actions
runs mypy and these tests on pull requests and pushes to `main`.

The original live embedding test is kept separately and skipped by default.
To run it intentionally with provider credentials configured (may incur API costs):

```bash
uv run pytest --run-integration -m integration tests/integration
```

## Local commit checks

After cloning the repository, install the development dependencies and activate
Git's pre-commit hook in that checkout:

```bash
uv sync --locked
uv run pre-commit install
```

On each commit, the hooks check staged files: Ruff applies safe lint fixes and
formats Python code, whitespace hooks remove trailing spaces and ensure a final
newline, and YAML/TOML hooks validate syntax. Markdown hard line breaks are
preserved. External source data and generated documents, indexes, and version
metadata are excluded.

Run the hooks on all tracked files manually with:

```bash
uv run pre-commit run --all-files
```

If a hook changes files, review the changes, stage them again, and retry the
commit. Each developer must run the install command in their own checkout;
committing the configuration does not activate hooks elsewhere. The first run
requires network access to install the isolated hook environments.

Keep the Ruff `rev` in `.pre-commit-config.yaml` aligned with the version in
`uv.lock` when upgrading Ruff. Mypy and pytest remain separate local commands
and CI checks; the existing GitHub checks still run even if local hooks are skipped.
