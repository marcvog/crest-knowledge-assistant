from crest_knowledge_assistant.file_utils import get_file_paths


def test_recursive_files_exclude_git(tmp_path):
    for name in ("root.h", "src/nested.cxx", ".git/config", "src/.git/HEAD"):
        path = tmp_path / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("fixture")
    assert set(get_file_paths(tmp_path)) == {
        tmp_path / "root.h",
        tmp_path / "src/nested.cxx",
    }


def test_empty_directory(tmp_path):
    assert get_file_paths(tmp_path) == []
