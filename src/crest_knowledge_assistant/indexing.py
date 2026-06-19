import os
from pathlib import Path
import sys



def main():
    # Get the current working directory
    cwd = Path(os.getcwd())
    
    # Get the parent directory of the current working directory
    parent_dir = cwd.parent
    
    # Add the parent directory to sys.path
    sys.path.append(str(parent_dir))
    
    # Now you can import modules from the parent directory
    from crest_knowledge_assistant.indexing import some_module  # Replace with actual module name

if __name__ == "__main__":
    main()