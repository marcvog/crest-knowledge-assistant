import os
from pathlib import Path
from uuid import uuid4
from tree_sitter import Parser
import tree_sitter_cpp
import hashlib
from datetime import datetime, UTC
from git import Repo

from tree_sitter import Language

from SemanticEntity import SemanticEntity, EntityKind
from IndexVersion import IndexVersion
from IndexStore import PROJECT_ROOT, INDEX_DIR, IndexStore

DATA_DIR = PROJECT_ROOT / "data"

# note that Parser is only an attribute, not a parent class
class LanguageParser:
    def __init__(self, language):
        self.parser = Parser(language)

    def parse(self, code):
        return self.parser.parse(code)


def first_child_of_type(node, node_type):
    for child in node.children:
        if child.type == node_type:
            return child
    return None


def find_by_field(node, field_name):
    results = []

    for i, child in enumerate(node.children):
        if node.field_name_for_child(i) == field_name:
            results.append(child)

        results.extend(find_by_field(child, field_name))

    return results


def get_file_paths(folder):
    file_paths = []
    for (dirpath, dirnames, filenames) in os.walk(folder):
        dirnames[:] = [d for d in dirnames if d not in ['.git']]
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            file_paths.append(file_path)
    return file_paths


def print_node(node):
    print('-----node description--------')
    print(f'Node type: {node.type}')
    print(node)
    print(f'Node start line: {node.start_point[0] + 1}')
    print(f'Node end line: {node.end_point[0] + 1}')
    print(f'Node text: {node.text.decode("utf-8")}')


def get_git_reference(project_root: Path) -> str:
    repo = Repo(project_root)
    return repo.head.commit.hexsha

class EntityExtractor:
    def __init__(self, path: Path):
        self.path = path
        self.entities: list[SemanticEntity] = []
        self.namespace_stack: list[str] = []
        self.class_stack: list[str] = []
        self.struct_stack: list[str] = []
        self.store = IndexStore(entity_path=INDEX_DIR / f"{path.stem}{path.suffix}.jsonl")

    def extract_namespace(self):
        """
        Extracts the current namespace from the namespace stack.
        """
        if self.namespace_stack:
            return "::".join(self.namespace_stack)
        return None


    def extract_name(self, node):
        """
        Extracts the name of a method from a qualified_identifier node.
        """
        if node.type == "qualified_identifier":
            name_node = node.child_by_field_name("name")
            if name_node.type == "qualified_identifier":
                return self.extract_name(name_node)
            elif name_node.type in {"identifier", "destructor_name"}:
                return name_node.text.decode()
        return None


    def extract_qualified_name(self, node):
        """
        Extracts the qualified name from a qualified_identifier node.
        """
        if node.type == "qualified_identifier":
            scope_node = node.child_by_field_name("scope")
            scope = scope_node.text.decode() if scope_node.type == "namespace_identifier" else None
            if scope:
                name_node = node.child_by_field_name("name")  
                if name_node.type == "qualified_identifier":
                    return scope + "::" + self.extract_qualified_name(name_node)
                elif name_node.type in {"identifier", "destructor_name"}:
                    return f"{scope}::{name_node.text.decode()}"
        return None   


    def print_function_type(self, node):
        print('-----Function type-----')
        type_node = node.child_by_field_name("type")
        if type_node is not None:
            print(f'node type: {type_node.type}')
            if type_node.type == "primitive_type":
                print(f'Primitive Type: {type_node.text.decode()}')
            elif type_node.type == "type_identifier":
                print(f'Type Identifier: {type_node.text.decode()}')
            elif type_node.type == "struct_specifier":
                print(f'Struct Specifier: {type_node.text.decode()}')
            elif type_node.type == "qualified_identifier":
                namespace_identifier = type_node.child_by_field_name("scope")
                print(f'Namespace Identifier: {namespace_identifier.text.decode()}')   
                type_identifier=type_node.child_by_field_name("name")
                print(f'Type Identifier: {type_identifier.text.decode()}')
            else:
                print(f'Unknown node type: {type_node.type}')
        else:
            print('This function does not have a type')


    def print_base_class(self, node):
        base_class_clause_node = first_child_of_type(node, "base_class_clause")
        if base_class_clause_node is not None:
            access_specifier_node = first_child_of_type(base_class_clause_node, "access_specifier")
            if access_specifier_node.type == "access_specifier":
                print(f'access_specifier_node text: {access_specifier_node.text.decode("utf-8")}')
            type_identifier_node = first_child_of_type(base_class_clause_node, "type_identifier")


            if type_identifier_node.type == "type_identifier":
                print(f'type_identifier_node text: {type_identifier_node.text.decode("utf-8")}')
        else:
            print('This class does not inherit from a base class')


    def process_node(self, node, path):

        print_node(node)
            
        if node.type == "function_definition":
            entity = self.process_function_node(node, path)
            print(f'Entity: {entity}')
            return entity

        elif node.type == "enum_specifier":
            entity = self.process_enum_node(node, path)
            print(f'Entity: {entity}')
            return entity
        
        elif node.type == "struct_specifier":
            entity = self.process_struct_node(node, path)
            print(f'Entity: {entity}')
            return entity
        
        elif node.type == "class_specifier":
            entity = self.process_class_node(node, path)
            print(f'Entity: {entity}')
            return entity

        return None


    def process_function_node(self, node, path):

        source_bytes = path.read_bytes()
        body = node.child_by_field_name("body")

        if node.type == "function_definition":
            self.print_function_type(node)

            # declarators = find_by_field(node, "declarator")
            # for node in declarators:
            #     print(node.type, node.text)

            identifier_node = None
            declarator_node = node.child_by_field_name("declarator")
            if declarator_node.type == "function_declarator":
                identifier_node = declarator_node.child_by_field_name("declarator")
            elif declarator_node.type in {"reference_declarator", "pointer_declarator"}:
                function_declarator_node = first_child_of_type(declarator_node, "function_declarator")             
                identifier_node = function_declarator_node.child_by_field_name("declarator")

            if identifier_node.type == "qualified_identifier": #class method
                kind = EntityKind.METHOD
                name=self.extract_name(identifier_node)
                parts = []
                parts.extend(self.namespace_stack)
                qualified_name = self.extract_qualified_name(identifier_node)
                if qualified_name:
                    parts.append(qualified_name)
                fully_qualified_name = "::".join(parts)
                print(f'Fully qualified name: {fully_qualified_name}')

            elif identifier_node.type == "identifier": # free functions and inline constructors
                if len(self.class_stack)>0:
                    kind = EntityKind.METHOD
                else:
                    kind = EntityKind.FUNCTION
                name = identifier_node.text.decode()
                parts = []
                parts.extend(self.namespace_stack)
                parts.extend(self.class_stack)
                parts.extend(self.struct_stack)
                if name:
                    parts.append(name)
                fully_qualified_name = "::".join(parts)
                print(f'Fully qualified name: {fully_qualified_name}')

            elif identifier_node.type == "field_identifier": # field or method member of a class/struct or union
                kind = EntityKind.METHOD
                name = identifier_node.text.decode()
                parts = []
                parts.extend(self.namespace_stack)
                parts.extend(self.class_stack)
                parts.extend(self.struct_stack)
                if name:
                    parts.append(name)
                fully_qualified_name = "::".join(parts)
                print(f'Fully qualified name: {fully_qualified_name}')

            elif identifier_node.type == "operator_name": # operator overload
                if len(self.class_stack)>0:
                    kind = EntityKind.METHOD
                else:
                    kind = EntityKind.FUNCTION
                name = identifier_node.text.decode()
                parts = []
                parts.extend(self.namespace_stack)
                parts.extend(self.class_stack)
                parts.extend(self.struct_stack)
                if name:
                    parts.append(name)
                fully_qualified_name = "::".join(parts)
                print(f'Fully qualified name: {fully_qualified_name}')

            elif identifier_node.type == "destructor_name":# inline destructor 
                kind = EntityKind.METHOD
                name = identifier_node.text.decode()
                parts = []
                parts.extend(self.namespace_stack)
                parts.extend(self.class_stack)
                parts.extend(self.struct_stack)
                if name:
                    parts.append(name)
                fully_qualified_name = "::".join(parts)
                print(f'Fully qualified name: {fully_qualified_name}')           

            end = (
                node.child_by_field_name("body")
                or next(
                    (
                        child
                        for child in node.children
                        if child.type in {
                            "default_method_clause",
                            "delete_method_clause",
                        }
                    ),
                    None,
                )
            )
            # unique identifier
            id=hashlib.sha256(
                fully_qualified_name.encode("utf-8")
                + b"\0"
                + source_bytes[node.start_byte : end.start_byte]
            ).hexdigest()
            
            entity = SemanticEntity(
                id=id,
                kind=kind,
                name=name,
                qualified_name=fully_qualified_name,
                namespace=self.extract_namespace(),
                source_file=path,
                start_line=node.start_point[0] + 1,
                end_line=node.end_point[0] + 1,
                signature=source_bytes[node.start_byte : end.start_byte].decode("utf-8"),
                documentation=None,
                source_code=node.text.decode("utf-8")
            )
            return entity

        return None


    def process_class_node(self, node, path):

        source_bytes = path.read_bytes()
        body = node.child_by_field_name("body")

        if node.type == "class_specifier":
            name_node = node.child_by_field_name("name")
            name = name_node.text.decode("utf-8") if name_node.type == "type_identifier" else None
            print(f'Class name: {name}')
            #self.print_base_class(node) # identify the base class if any
            parts = []
            parts.extend(self.namespace_stack)
            parts.extend(self.class_stack)
            if name:
                parts.append(name)
            fully_qualified_name = "::".join(parts)
            print(f'Fully qualified class name: {fully_qualified_name}')

            # unique identifier
            id=hashlib.sha256(
                fully_qualified_name.encode("utf-8")
                + b"\0"
                + source_bytes[node.start_byte : body.start_byte]
            ).hexdigest()

            entity = SemanticEntity(
                id=id,
                kind=EntityKind.CLASS,
                name=name,
                qualified_name=fully_qualified_name,
                namespace=self.extract_namespace(),
                source_file=path,
                start_line=node.start_point[0] + 1,
                end_line=node.end_point[0] + 1,
                signature=source_bytes[node.start_byte : body.start_byte].decode("utf-8"),
                documentation=None,
                source_code=node.text.decode("utf-8")
            )
            return entity
        
        return None


    def process_struct_node(self, node, path):

        source_bytes = path.read_bytes()
        body = node.child_by_field_name("body")

        if node.type == "struct_specifier":
            name_node = node.child_by_field_name("name")
            name = name_node.text.decode("utf-8") if name_node.type == "type_identifier" else None
            print(f'Struct name: {name}')
            parts = []
            parts.extend(self.namespace_stack)
            parts.extend(self.class_stack)
            if name:
                parts.append(name)
            fully_qualified_name = "::".join(parts)
            print(f'Fully qualified struct name: {fully_qualified_name}')

            # unique identifier
            id=hashlib.sha256(
                fully_qualified_name.encode("utf-8")
                + b"\0"
                + source_bytes[node.start_byte : body.start_byte]
            ).hexdigest()

            entity = SemanticEntity(
                id=id,
                kind=EntityKind.STRUCT,
                name=name,
                qualified_name=fully_qualified_name,
                namespace=self.extract_namespace(),
                source_file=path,
                start_line=node.start_point[0] + 1,
                end_line=node.end_point[0] + 1,
                signature=source_bytes[node.start_byte : body.start_byte].decode("utf-8"),
                documentation=None,
                source_code=node.text.decode("utf-8")
            )
            return entity
        
        return None


    def process_enum_node(self, node, path):

        source_bytes = path.read_bytes()
        body = node.child_by_field_name("body")

        if node.type == "enum_specifier":
            name_node = node.child_by_field_name("name")
            name = name_node.text.decode("utf-8") if name_node.type == "type_identifier" else None
            print(f'Enum name: {name}')
            parts = []
            parts.extend(self.namespace_stack)
            parts.extend(self.class_stack)
            if name:
                parts.append(name)
            fully_qualified_name = "::".join(parts)
            print(f'Fully qualified enum name: {fully_qualified_name}')

            # unique identifier
            id=hashlib.sha256(
                fully_qualified_name.encode("utf-8")
                + b"\0"
                + source_bytes[node.start_byte : body.start_byte]
            ).hexdigest()

            entity = SemanticEntity(
                id=id,
                kind=EntityKind.ENUM,
                name=name,
                qualified_name=fully_qualified_name,
                namespace=self.extract_namespace(),
                source_file=path,
                start_line=node.start_point[0] + 1,
                end_line=node.end_point[0] + 1,
                signature=source_bytes[node.start_byte : body.start_byte].decode("utf-8"),
                documentation=None,
                source_code=node.text.decode("utf-8")
            )
            return entity
        
        return None


    def walk(self, node, level=0):

        entity = None
        print("  " * level + node.type)

        if node.type == "namespace_definition":
            name_node = node.child_by_field_name("name")
            namespace = name_node.text.decode() if name_node.type == "namespace_identifier" else None
            print(f'Namespace Identifier text: {namespace}')
            self.namespace_stack.append(namespace)

            for child in node.children:
                self.walk(child)

            print("  " * level + f"Exiting namespace: {namespace}")
            self.namespace_stack.pop()
            return

        if node.type == "class_specifier":
            name_node = node.child_by_field_name("name")
            name = name_node.text.decode("utf-8") if name_node.type == "type_identifier" else None
            if name:
                print(f'Class name: {name}')
                entity = self.process_node(node, self.path)
                if entity is not None:
                    self.entities.append(entity)
                self.class_stack.append(name)

                for child in node.children:
                    self.walk(child)

                print("  " * level + f"Exiting class: {name}")
                self.class_stack.pop()
                return

        if node.type == "struct_specifier":
            body = node.child_by_field_name("body")
            if body is None:
                # This is just a struct used as a type.
                return

            name_node = node.child_by_field_name("name")
            name = name_node.text.decode("utf-8") if name_node.type == "type_identifier" else None
            if name:
                print(f'Struct name: {name}')
                entity = self.process_node(node, self.path)
                if entity is not None:
                    self.entities.append(entity)
                self.struct_stack.append(name)

                for child in node.children:
                    self.walk(child)

                print("  " * level + f"Exiting struct: {name}")
                self.struct_stack.pop()
                return
                    
        entity = self.process_node(node, self.path)
        if entity is not None:
            self.entities.append(entity)

        for child in node.children:
            self.walk(child)


def main():
    # Get the current working directory
    #cwd = Path(os.getcwd())
    
    # Get the parent directory of the current working directory
    #parent_dir = cwd.parent
    
    # Add the parent directory to sys.path
    #sys.path.append(str(parent_dir))
    
    # Now you can import modules from the parent directory
    #from crest_knowledge_assistant.indexing import some_module  # Replace with actual module name
    folder = DATA_DIR / "CrestApi"

    file_paths = get_file_paths(folder.resolve())
    file_paths = [Path(file_path) for file_path in file_paths if ".cxx" in file_path or ".h" in file_path]
    #file_paths = [Path(file_path) for file_path in file_paths if ".cxx" in file_path]
    print(file_paths)

    # Source file
    #path = Path("../../data/CrestApi/src/CrestApi.cxx")
    #path = Path("../../data/CrestApi/CrestApi/CrestApi.h")
    #path = Path("../../data/CrestApi/CrestApi/CrestRequest.h")
    #path = Path("../../data/CrestApi/CrestApi/ChannelSetDto.h")
    #path = Path("../../data/CrestApi/src/CrestRequest.cxx")
    #path = Path("../../data/CrestApi/tools/CrestContainer.cxx")
    #path = Path("../../data/CrestApi/test/test_utils.h")
    
    language = Language(tree_sitter_cpp.language())
    parser = LanguageParser(language)

    # tree = parser.parse(path.read_bytes())
    # print(tree.root_node.type)
    # extractor = EntityExtractor(path)
    # extractor.walk(tree.root_node)
    # print(f'entities length: {len(extractor.entities)}')
    # extractor.store.save_entities(extractor.entities)

    entity_count = 0
    for path in file_paths:
        tree = parser.parse(path.read_bytes())
        print(tree.root_node.type)
        extractor = EntityExtractor(path)
        extractor.walk(tree.root_node)
        entity_count += len(extractor.entities)
        extractor.store.save_entities(extractor.entities)

    print(f'Total entities extracted: {entity_count}')
    index_version = IndexVersion(
        version_id=str(uuid4()),
        git_reference=get_git_reference(PROJECT_ROOT),
        indexing_date=datetime.now(UTC).isoformat(),
        entity_count=entity_count
    )

    extractor.store.save_version(index_version)

if __name__ == "__main__":
    main()