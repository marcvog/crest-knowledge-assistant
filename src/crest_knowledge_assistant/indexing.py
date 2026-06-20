import os
from pathlib import Path
import pathlib
import sys

def get_file_paths(folder):
    file_paths = []
    for (dirpath, dirnames, filenames) in os.walk(folder):
        dirnames[:] = [d for d in dirnames if d not in ['.git']]
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            file_paths.append(file_path)
    return file_paths

def main():
    # Get the current working directory
    #cwd = Path(os.getcwd())
    
    # Get the parent directory of the current working directory
    #parent_dir = cwd.parent
    
    # Add the parent directory to sys.path
    #sys.path.append(str(parent_dir))
    
    # Now you can import modules from the parent directory
    #from crest_knowledge_assistant.indexing import some_module  # Replace with actual module name
    folder = '../data/CrestApi'

    file_paths = get_file_paths(folder)

    print(file_paths)
    
    '''
    path = pathlib.Path(folder)
    #print(path)
    for path in path.rglob('**/*'):
        if path.is_file():
            print(path)
    '''
if __name__ == "__main__":
    main()