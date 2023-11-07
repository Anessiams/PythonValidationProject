# example policy

import sys
#from msc2 import validate
#from msc2 import additional_file_path
#from msc2 import hello_file_path


def main():
    if len(sys.argv) == 2:
        file_path = sys.argv[1]
        with open(file_path, 'r') as file:
            content = file.read()
        print(f"Hello, the contents of the additional file are: \n{content}")
        #validate(additional_file_path, hello_file_path)
    else:
        print("Usage: hello.py <path_to_additional_file>")

if __name__ == "__main__":
    main()
