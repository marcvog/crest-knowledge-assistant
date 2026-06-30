import code
from enum import Enum
from html import parser
import os
from pathlib import Path
import pathlib
from platform import node
import sys
from tree_sitter import Parser
import tree_sitter_cpp
from dataclasses import dataclass

from tree_sitter import Language, Query, QueryCursor


namespace_stack = []

def get_file_paths(folder):
    file_paths = []
    for (dirpath, dirnames, filenames) in os.walk(folder):
        dirnames[:] = [d for d in dirnames if d not in ['.git']]
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            file_paths.append(file_path)
    return file_paths

class EntityKind(Enum):
    NAMESPACE = "namespace"
    CLASS = "class"
    STRUCT = "struct"
    METHOD = "method"
    FUNCTION = "function"
    ENUM = "enum"
    TYPE_ALIAS = "type_alias"

@dataclass
class SemanticEntity:
    kind: EntityKind
    name: str
    qualified_name: str

    namespace: str | None
    owner: str | None

    source_file: Path
    start_line: int
    end_line: int

    signature: str
    documentation: str | None
    source_code: str

entities = [SemanticEntity]

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
        name = extract_name(node)
        if scope and name:
            return f"{scope}::{name}"
    return None

def extract_scope(node):
    """
    Extracts the scope from a qualified_identifier node.
    """
    if node.type == "qualified_identifier":
        scope_node = node.child_by_field_name("scope")
        if scope_node.type == "namespace_identifier":
            scope = scope_node.text.decode()
            return scope
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
            #identifier_node = name_node.named_child(0)
            return name_node.text.decode()
    return None

def extract_namespace(node):
    """
    Extracts the namespace from a namespace_definition node.
    """
    if node.type == "namespace_definition":
        name_node = node.child_by_field_name("name")
        if name_node.type == "namespace_identifier":
            return name_node.text.decode()
    return None

def process_node(node, code):
    """
    entity = SemanticEntity(
        kind=EntityKind.FUNCTION,
        name=
    )
    """
    """
    if node.type == "namespace_definition":
        print(node)
        namespace_identifier = node.child_by_field_name("name")
        print(f'Namespace Identifier text: {namespace_identifier.text.decode()}')
    """
        
    if node.type == "function_definition":
        # Extract function name, parameters, return type, etc.
        # You can use node.child_by_field_name("name") to get the function name node
        # and node.child_by_field_name("parameters") to get the parameters node
        # Similarly, you can extract other relevant information


        parts = []
        parts.extend(namespace_stack)
        qualified_name = extract_qualified_name(extract_qualified_identifier_node(node))
        if qualified_name:
            parts.append(qualified_name)
        fully_qualified_name = "::".join(parts)
        print(f'Fully qualified name: {fully_qualified_name}')




        print(f'Node type: {node.type}')
        print(parts)
        print(node)
        print(f'Function start line: {node.start_point[0] + 1}')
        print(f'Function end line: {node.end_point[0] + 1}')

        qualified_identifier_node = extract_qualified_identifier_node(node)
        if qualified_identifier_node.type == "qualified_identifier":
            qualified_name = extract_qualified_name(qualified_identifier_node)
            print(f'Qualified name: {qualified_name}')


        
        print('-------------')
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

        print(f'Node text: {node.text.decode()}')

        body = node.child_by_field_name("body")
        signature = code[node.start_byte : body.start_byte].decode()
        print(f'Signature: {signature}')

        #qualified_name = 
        

        #print(f'Declarator: {declarator}')
        #print(f'Body: {body}')
        #print(f'Type: {type_node}')

        pass

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
    #print(file_paths)

    cpp_language = Language(tree_sitter_cpp.language())


    parser = Parser(cpp_language)

    # Read the source file as bytes
    code = Path("../data/CrestApi/src/CrestApi.cxx").read_bytes()
    #code = Path("../data/CrestApi/CrestApi/CrestApi.h").read_bytes()
    
    tree = parser.parse(code)
    print(tree.root_node.type)

    """
    (class_specifier) @class
    (struct_specifier) @struct
    (enum_specifier) @enum
    (function_definition) @function
     (namespace_definition) @namespace
    """


    query = Query(cpp_language,
        #"(function_definition) @function",
        "(class_specifier) @class"
    )
    cursor = QueryCursor(query)
    captures = cursor.captures(tree.root_node)

    #print(type(captures))
    #print(captures)
    """
    for function_node in captures["function"]:
        print(function_node.type)
        print(function_node.text.decode("utf-8"))
    """

    """
    for capture_name, nodes in captures.items():
        for node in nodes:
            print(capture_name, node.text.decode("utf-8"))
    """

    """
    def dump(node, code, indent=0):
        print(
            "  " * indent,
            node.type,
            repr(code[node.start_byte:node.end_byte][:40]),
        )

        for child in node.children:
            dump(child, code, indent + 1)

    dump(tree.root_node, code)
    """

    """
    def walk(node, level=0):
        print("  " * level + node.type)
        process_node(node,code)
        for child in node.children:
            walk(child, level + 1)

    walk(tree.root_node)
    """

    def walk(node, level=0):

        print("  " * level + node.type)

        if node.type == "namespace_definition":

            namespace = extract_namespace(node)
            print(f'Namespace Identifier text: {namespace}')

            namespace_stack.append(namespace)

            for child in node.children:
                walk(child)

            print("  " * level + f"Exiting namespace: {namespace}")
            namespace_stack.pop()

            return

        process_node(node, code)

        for child in node.children:
            walk(child)
    
    walk(tree.root_node)

if __name__ == "__main__":
    main()