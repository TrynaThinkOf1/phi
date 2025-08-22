"""
 PHI
 7/8/2025

 Phi C++ Project
 sysutils.py

 Zevi Berlin
"""

from datetime import datetime
from os import path, makedirs


def create_code_files(header_parent_dir: str, source_parent_dir: str, filename: str, author_name: str) -> None:
    author_name = "Zevi Berlin" if not author_name else author_name
    
    cap_name: str = filename.upper()
    date: str = str(datetime.now()).split(" ")[0].replace("-", "/")

    header_path: str = f"include/{header_parent_dir + '/' if header_parent_dir else ''}{filename}.hpp"
    
    header_content: str = f"""/*

 PHI
 {date}

 Phi C++ Project
 {header_path}

 {author_name}

*/

#ifndef {cap_name}_HPP
#define {cap_name}_HPP



#endif /* {cap_name}_HPP */

"""

    makedirs(f"include/{header_parent_dir}", exist_ok=True)

    if path.exists(header_path) and path.getsize(header_path) > 0:
        print(f"Header file '{header_path}' already exists and has content.")
        if input("Would you like to erase and rewrite it (y/n)? ") not in ["y", "Y", "yes", "YES", "1", 1]:
            print("\nExiting.")
            exit(0)
            
    with open(header_path, "w") as header:
        header.write(header_content)
        

    source_path: str = f"src/{source_parent_dir + '/' if source_parent_dir else ''}{filename}.cpp"

    source_content: str = f"""/*

 PHI
 {date}

 Phi C++ Project
 {source_path}

 {author_name}

*/

#include "{header_parent_dir + '/' if header_parent_dir else ''}{filename}.hpp"

"""
    makedirs(f"src/{source_parent_dir}", exist_ok=True)
    
    if path.exists(source_path) and path.getsize(source_path) > 0:
        print(f"Source file '{source_path}' already exists and has content.")
        if input("Would you like to erase and rewrite it (y/n)? ") not in ["y", "Y", "yes", "YES", "1", 1]:
            print("\nExiting.")
            exit(0)

    with open(source_path, "w") as source:
        source.write(source_content)


def main():
    menu: str = """OPTIONS:
  1) Create Code Files
      Add a source file and corresponding header file. Also boilerplates with include guards and header comment.

  2) Exit
"""
    print(menu)
    #x = input("\n    >> ")
    match (x := input("\n    >> ")):
        case "1":
            header_parent_dir = input("Header Parent Dir (e.g 'data' for path 'include/data/'): ")
            source_parent_dir = input("Source Parent Dir (e.g 'networking' for path 'src/networking/'): ")
            filename = input("Filename (do not include .cpp/.hpp): ")
            author_name = input("Your name (defaults to Zevi Berlin): ")
            create_code_files(header_parent_dir, source_parent_dir, filename, author_name)
            print("\nSUCCESSFULLY CREATED AND BOILERPLATED FILES")
            exit(0)

        case "2":
            print("\nExiting.")
            exit(0)

        case _:
            print(f"Input '{x}' not recognized.")
            exit(1)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nExiting.")
        exit(0)
