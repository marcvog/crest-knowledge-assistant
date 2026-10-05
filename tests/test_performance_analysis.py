import json

import pytest

pytest.importorskip("pandas", reason="Install the performance group to test analysis")

from crest_knowledge_assistant.performance.analysis import load_run


@pytest.fixture
def profile_dir(tmp_path):
    (tmp_path / "metadata.json").write_text(
        json.dumps({"schema_version": 1, "scalene_version": "2.3.0"})
    )
    return tmp_path


def write_profile(directory, data):
    (directory / "profile.json").write_text(json.dumps(data))


def test_load_measurements_and_missing_values(profile_dir):
    write_profile(
        profile_dir,
        {
            "memory": True,
            "files": {
                "example.py": {
                    "lines": [
                        {"lineno": 3, "n_cpu_percent_python": 12.5, "n_peak_mb": 0}
                    ]
                }
            },
        },
    )
    runs, lines = load_run(profile_dir)
    assert len(runs) == 1
    assert lines.loc[0, "cpu_python_percent"] == 12.5
    assert lines.loc[0, "peak_mb"] == 0
    assert lines["allocated_mb"].isna().all()


def test_cpu_only_memory_is_missing(profile_dir):
    write_profile(
        profile_dir,
        {
            "memory": False,
            "files": {"example.py": {"lines": [{"lineno": 1, "n_peak_mb": 0}]}},
        },
    )
    _, lines = load_run(profile_dir)
    assert lines["peak_mb"].isna().all()


def test_empty_profile(profile_dir):
    write_profile(profile_dir, {"files": {}})
    runs, lines = load_run(profile_dir)
    assert len(runs) == 1
    assert lines.empty
    assert "cpu_python_percent" in lines.columns


@pytest.mark.parametrize("data", [{}, {"files": []}, {"files": {"x": {"lines": [{}]}}}])
def test_invalid_schema(profile_dir, data):
    write_profile(profile_dir, data)
    with pytest.raises(ValueError, match=r"files mapping|Missing line number"):
        load_run(profile_dir)
