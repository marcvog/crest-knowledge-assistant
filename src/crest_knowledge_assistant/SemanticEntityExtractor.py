import os
from pathlib import Path
from tree_sitter import Parser
import tree_sitter_cpp

from tree_sitter import Language, Query, QueryCursor

from SemanticEntity import SemanticEntity, EntityKind
from SemanticEntityStore import EntityStore

store = EntityStore()
namespace_stack = []
struct_stack = []
class_stack = []


# note that Parser is only an attribute, not a parent class
class LanguageParser:
    def __init__(self, language):
        self.parser = Parser(language)

    def parse(self, code):
        return self.parser.parse(code)


class EntityExtractor:
    def __init__(self):
        pass


def first_child_of_type(node, node_type):
    for child in node.children:
        if child.type == node_type:
            return child
    return None

def get_file_paths(folder):
    file_paths = []
    for (dirpath, dirnames, filenames) in os.walk(folder):
        dirnames[:] = [d for d in dirnames if d not in ['.git']]
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            file_paths.append(file_path)
    return file_paths

entities: list[SemanticEntity] = []


def extract_qualified_identifier_node(node):
    """
    Extracts the qualified_identifier node from a function_definition node.
    """

    if node.type == "function_definition":
        function_declarator_node = node.child_by_field_name("declarator")
        qualified_identifier_node = function_declarator_node.child_by_field_name("declarator")
        if qualified_identifier_node.type == "qualified_identifier":
            return qualified_identifier_node
    return None

def extract_qualified_name(node):
    """
    Extracts the qualified name from a qualified_identifier node.
    """
    if node.type == "qualified_identifier":
        scope = extract_scope(node)
        name_node = node.child_by_field_name("name")  
        if name_node.type == "qualified_identifier":
            return scope + "::" + extract_qualified_name(name_node)
        elif name_node.type == "identifier" or "destructor_name":
            return f"{scope}::{name_node.text.decode()}"
    return None

def extract_scope(node):
    """
    Extracts the scope from a qualified_identifier node.
    """
    if node.type == "qualified_identifier":
        scope_node = node.child_by_field_name("scope")
        if scope_node.type == "namespace_identifier":
            return scope_node.text.decode()
    return None

def extract_name(node):
    """
    Extracts the name of a function from a qualified_identifier node.
    """
    if node.type == "qualified_identifier":
        name_node = node.child_by_field_name("name")
        if name_node.type == "identifier":
            return name_node.text.decode()
        elif name_node.type == "destructor_name":
            return name_node.text.decode()
        elif name_node.type == "qualified_identifier":
            return extract_name(name_node)
    return None

def extract_free_function_name(node):#this is not being used
    """
    Extracts the name of a function from a function_definition node.
    """

    if node.type == "function_definition":
        function_declarator_node = node.child_by_field_name("declarator")
        identifier_node = function_declarator_node.child_by_field_name("declarator")
        if identifier_node.type == "identifier":
            return identifier_node.text.decode()
    return None

def extract_namespace_from_node(node):
    """
    Extracts the namespace from a namespace_definition node.
    """
    if node.type == "namespace_definition":
        name_node = node.child_by_field_name("name")
        if name_node.type == "namespace_identifier":
            return name_node.text.decode()
    return None

def extract_namespace():
    """
    Extracts the current namespace from the namespace stack.
    """
    if namespace_stack:
        return "::".join(namespace_stack)
    return None

def extract_parent(node):#this is not being used
    """
    Extracts the parent of a semantic entity.
    """
    if node.type == "function_definition":
        qualified_identifier_node = extract_qualified_identifier_node(node)
        parts = []
        parts.extend(namespace_stack)
        scope =extract_scope(qualified_identifier_node)
        if scope:
            parts.append(scope)
        parent = "::".join(parts)
        return parent
    #elif node.type == "class_specifier":
    return None    


def process_node(node, path):
    print('-----node description--------')
    print(f'Node type: {node.type}')
    print(node)
    
    print(f'Node start line: {node.start_point[0] + 1}')
    print(f'Node end line: {node.end_point[0] + 1}')
    print(f'Node text: {node.text.decode("utf-8")}')
    print(f'Source file: {path}')

    source_bytes = path.read_bytes()
    body = node.child_by_field_name("body")
    #print(f'Body: {body}')
        
    if node.type == "function_definition":
        # Extract function name, parameters, return type, etc.
        # You can use node.child_by_field_name("name") to get the function name node
        # and node.child_by_field_name("parameters") to get the parameters node
        # Similarly, you can extract other relevant information

        qualified_identifier_node = extract_qualified_identifier_node(node)
        if qualified_identifier_node:
            kind = EntityKind.METHOD
            name=extract_name(qualified_identifier_node)
            parts = []
            parts.extend(namespace_stack)
            qualified_name = extract_qualified_name(qualified_identifier_node)
            if qualified_name:
                parts.append(qualified_name)
            fully_qualified_name = "::".join(parts)
            print(f'Fully qualified name: {fully_qualified_name}')

        else:# this is the free function
            function_declarator_node = node.child_by_field_name("declarator")
            identifier_node = function_declarator_node.child_by_field_name("declarator")
            if identifier_node.type == "identifier":
                kind = EntityKind.FUNCTION
                name = identifier_node.text.decode()
                parts = []
                parts.extend(namespace_stack)

                if name:
                    parts.append(name)
                fully_qualified_name = "::".join(parts)
                print(f'Fully qualified name: {fully_qualified_name}')

            elif identifier_node.type == "field_identifier":# functions inside struct
                kind = EntityKind.METHOD
                name = identifier_node.text.decode()
                parts = []
                parts.extend(namespace_stack)
                parts.extend(class_stack)
                parts.extend(struct_stack)

                if name:
                    parts.append(name)
                fully_qualified_name = "::".join(parts)
                print(f'Fully qualified name: {fully_qualified_name}')


        print('-----return type block--------')
        node_type = node.child_by_field_name("type")
        if node_type is not None:
            print(node_type.type)
            if node_type.type == "primitive_type":
                print(f'Primitive Type: {node_type.text.decode()}')
            elif node_type.type == "type_identifier":
                print(f'Type Identifier: {node_type.text.decode()}')
            elif node_type.type == "qualified_identifier":
                namespace_identifier = node_type.child_by_field_name("scope")
                print(f'Namespace Identifier text: {namespace_identifier.text.decode()}')   
                type_identifier=node_type.child_by_field_name("name")
                print(f'Type Identifier text: {type_identifier.text.decode()}')
            else:
                print(f'Unknown node type: {node_type.type}')
        else:
            print('This function does not have a type')

        signature = source_bytes[node.start_byte : body.start_byte].decode("utf-8")
        print(f'Signature: {signature}')
        
        entity = SemanticEntity(
            kind=kind,
            name=name,
            qualified_name=fully_qualified_name,
            namespace=extract_namespace(),
            source_file=path,
            start_line=node.start_point[0] + 1,
            end_line=node.end_point[0] + 1,
            signature=source_bytes[node.start_byte : body.start_byte].decode("utf-8"),
            documentation=None,
            source_code=node.text.decode("utf-8")
        )
        print(f'Entity: {entity}')

        return entity

    elif node.type == "enum_specifier":# watch out, these can be inside classes and structs. Implement to skip in those cases for now

        name_node = node.child_by_field_name("name")
        if name_node.type == "type_identifier":
            print(f'Enum name: {name_node.text.decode("utf-8")}')
        else:
            print('This enum does not have a name')

        parts = []
        parts.extend(namespace_stack)
        parts.extend(class_stack)
        qualified_name = name_node.text.decode("utf-8")
        if qualified_name:
            parts.append(qualified_name)
        fully_qualified_name = "::".join(parts)
        print(f'Fully qualified enum name: {fully_qualified_name}')

        entity = SemanticEntity(
            kind=EntityKind.ENUM,
            name=name_node.text.decode("utf-8"),
            qualified_name=fully_qualified_name,
            namespace=extract_namespace(),
            source_file=path,
            start_line=node.start_point[0] + 1,
            end_line=node.end_point[0] + 1,
            signature=source_bytes[node.start_byte : body.start_byte].decode("utf-8"),
            documentation=None,
            source_code=node.text.decode("utf-8")
        )
        print(f'Entity: {entity}')
        return entity
    
    elif node.type == "struct_specifier":

        name_node = node.child_by_field_name("name")
        if name_node.type == "type_identifier":
            print(f'Struct name: {name_node.text.decode("utf-8")}')
        else:
            print('This struct does not have a name')

        parts = []
        parts.extend(namespace_stack)
        parts.extend(class_stack)
        qualified_name = name_node.text.decode("utf-8")
        if qualified_name:
            parts.append(qualified_name)
        fully_qualified_name = "::".join(parts)
        print(f'Fully qualified struct name: {fully_qualified_name}')

        entity = SemanticEntity(
            kind=EntityKind.STRUCT,
            name=name_node.text.decode("utf-8"),
            qualified_name=fully_qualified_name,
            namespace=extract_namespace(),
            source_file=path,
            start_line=node.start_point[0] + 1,
            end_line=node.end_point[0] + 1,
            signature=source_bytes[node.start_byte : body.start_byte].decode("utf-8"),
            documentation=None,
            source_code=node.text.decode("utf-8")
        )
        print(f'Entity: {entity}')
        return entity 
    
    elif node.type == "class_specifier":
        name_node = node.child_by_field_name("name")
        if name_node.type == "type_identifier":
            name = name_node.text.decode("utf-8")
            print(f'Class name: {name}')

        base_class_clause_node = first_child_of_type(node, "base_class_clause")
        if base_class_clause_node is not None:
            access_specifier_node = first_child_of_type(base_class_clause_node, "access_specifier")
            if access_specifier_node.type == "access_specifier":
                print(f'access_specifier_node text: {access_specifier_node.text.decode("utf-8")}')
            type_identifier_node = first_child_of_type(base_class_clause_node, "type_identifier")
            if type_identifier_node.type == "type_identifier":
                print(f'type_identifier_node text: {type_identifier_node.text.decode("utf-8")}')

        parts = []
        parts.extend(namespace_stack)
        parts.extend(class_stack)
        qualified_name = name_node.text.decode("utf-8")
        if qualified_name:
            parts.append(qualified_name)
        fully_qualified_name = "::".join(parts)
        print(f'Fully qualified class name: {fully_qualified_name}')


        entity = SemanticEntity(
            kind=EntityKind.CLASS,
            name=name_node.text.decode("utf-8"),
            qualified_name=fully_qualified_name,
            namespace=extract_namespace(),
            source_file=path,
            start_line=node.start_point[0] + 1,
            end_line=node.end_point[0] + 1,
            signature=source_bytes[node.start_byte : body.start_byte].decode("utf-8"),
            documentation=None,
            source_code=node.text.decode("utf-8")
        )
        print(f'Entity: {entity}')
        return entity 

    return None

def main():
    # Get the current working directory
    #cwd = Path(os.getcwd())
    
    # Get the parent directory of the current working directory
    #parent_dir = cwd.parent
    
    # Add the parent directory to sys.path
    #sys.path.append(str(parent_dir))
    
    # Now you can import modules from the parent directory
    #from crest_knowledge_assistant.indexing import some_module  # Replace with actual module name
    folder = '../../data/CrestApi'

    file_paths = get_file_paths(folder)
    #print(file_paths)

    # Source file
    path = Path("../../data/CrestApi/src/CrestApi.cxx")
    #path = Path("../../data/CrestApi/CrestApi/CrestApi.h")
    
    language = Language(tree_sitter_cpp.language())
    parser = LanguageParser(language)
    tree = parser.parse(path.read_bytes())
    print(tree.root_node.type)


    def walk(node, level=0):

        entity = None
        print("  " * level + node.type)

        if node.type == "namespace_definition":

            namespace = extract_namespace_from_node(node)
            print(f'Namespace Identifier text: {namespace}')

            namespace_stack.append(namespace)

            for child in node.children:
                walk(child)

            print("  " * level + f"Exiting namespace: {namespace}")
            namespace_stack.pop()

            return

        if node.type == "struct_specifier":
            name_node = node.child_by_field_name("name")
            if name_node.type == "type_identifier":
                print(f'Struct name: {name_node.text.decode("utf-8")}')
                entity=process_node(node, path)
                if entity is not None:
                    print(f'Entity: {entity}')
                    entities.append(entity)
                struct_stack.append(name_node.text.decode("utf-8"))

                for child in node.children:
                    walk(child)

                print("  " * level + f"Exiting struct: {name_node.text.decode("utf-8")}")
                struct_stack.pop()

                return
            
        if node.type == "class_specifier":
            name_node = node.child_by_field_name("name")
            if name_node.type == "type_identifier":
                print(f'Class name: {name_node.text.decode("utf-8")}')
                entity=process_node(node, path)
                if entity is not None:
                    print(f'Entity: {entity}')
                    entities.append(entity)
                class_stack.append(name_node.text.decode("utf-8"))


                for child in node.children:
                    walk(child)

                print("  " * level + f"Exiting class: {name_node.text.decode("utf-8")}")
                class_stack.pop()

                return
         

        entity=process_node(node, path)
        if entity is not None:
            print(f'Entity: {entity}')
            entities.append(entity)

        for child in node.children:
            walk(child)

    
    walk(tree.root_node)
    print(f'entities length: {len(entities)}')
    store.save(entities)

if __name__ == "__main__":
    main()