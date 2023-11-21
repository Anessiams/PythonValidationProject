import sys

# Check input files if they contain the illegal words


def main():
    dirtywords = ["secret", "secrets", "secretest"]

    # read the file
    if len(sys.argv) == 2:
        file_path = sys.argv[1]
        with open(file_path, 'r') as file:
            content = file.read()
    for each in dirtywords:
        if each in content:
            print("Banned words found")
            return 1
    print("No secrets found")
    return 0

# Wrapper for the validator: named 'validate'


def validate(data_file):
    return main()


if __name__ == "__main__":
    # to allow the script to be callable for command line
    validate(sys.argv[1])
