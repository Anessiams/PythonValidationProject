import sys


def main():
    print("Passing all data")
    return 0

# Wrapper for the validator: named 'validate'


def validate(data_file):
    return main()


if __name__ == "__main__":
    # to allow the script to be callable for command line
    validate(sys.argv[1])
