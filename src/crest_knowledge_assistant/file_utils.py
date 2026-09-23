from pathlib import Path


def get_file_paths(folder: Path) -> list[Path]:
    file_paths: list[Path] = []

    for dirpath, dirnames, filenames in folder.walk():
        dirnames[:] = [d for d in dirnames if d != ".git"]

        file_paths.extend(dirpath / filename for filename in filenames)

    return file_paths
