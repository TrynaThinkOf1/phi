"""
 PHI
 7/8/2025

 Phi C++ Project
 sysutils.py

 Zevi Berlin
"""

from datetime import datetime
from os import path, makedirs, system


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

#########################



#########################

def main():
    menu: str = """OPTIONS:
  1) Create Code Files
      Add a source file and corresponding header file. Also boilerplates with include guards and header comment.

  2) Build (Release)

  3) Build (Debug)

  4) Cleanup

  5) Format

  6) Analyze

  7) Exit
"""
    print(menu)

    match (x := int(input("\n    >> "))):
        case 1:
            header_parent_dir = input("Header Parent Dir (e.g 'data' for path 'include/data/'): ")
            source_parent_dir = input("Source Parent Dir ('phi' or 'phid'): ")
            filename = input("Filename (do not include .cpp/.hpp): ")
            author_name = input("Your name (defaults to Zevi Berlin): ")
            create_code_files(header_parent_dir, source_parent_dir, filename, author_name)
            print("\nSUCCESSFULLY CREATED AND BOILERPLATED FILES")

        case 2:
            system("cmake -S . -B _build -DCMAKE_BUILD_TYPE=Release")
            if input("Continue? ") != "y": exit(1)
            system("cmake --build _build -j4")
            
            system("mv _build/phi ./ && mv _build/phid ./")

        case 3:
            system("cmake -S . -B _build -DCMAKE_BUILD_TYPE=Debug")
            if input("Continue? ") != "y": exit(1)
            system("cmake --build _build -j4")
            
            system("mv _build/phi ./ && mv _build/phid ./")

            system("dsymutil ./phi && dsymutil ./phid")

        case 4:
            system("mv _build/compile_commands.json ./")

            system("rm -rf _build/")
            system("mkdir _build")

            system("mv ./compile_commands.json _build/")

            system("rm -f phi phid")
            system("rm -rf phi.dSYM phid.dSYM .cache")

        case 5:
            system("find src -type f \\( -name \"*.c\" -o -name \"*.cpp\" \\) -exec clang-format -i {} +")
            system("find include -type f \\( -name \"*.h\" -o -name \"*.hpp\" \\) -exec clang-format -i {} +")

        case 6:
            system("find src -type f \\( -name \"*.c\" -o -name \"*.cpp\" \\) -exec clang-tidy --config-file=./.clang-tidy --format-style=file -p _build {} +")

        case 7:
            print("\nExiting.")

        case _:
            print(f"Input '{x}' not recognized.")
            exit(1)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nExiting.")
        exit(0)
