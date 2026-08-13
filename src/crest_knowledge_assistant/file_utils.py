from pathlib import Path

def get_file_paths(folder: Path) -> list[Path]:
    file_paths: list[Path] = []

    for dirpath, dirnames, filenames in folder.walk():
        dirnames[:] = [d for d in dirnames if d != ".git"]

        for filename in filenames:
            file_paths.append(dirpath / filename)

    return file_paths