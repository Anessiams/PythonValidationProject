# Check input files if they contain the illegal words
def validate(data_file):
    dirtywords = ["secret", "secrets", "secretest"]
    # read the file
    for each in dirtywords:
        if each in data_file.split():
            print("Banned words found")
            return 0
    print("No secrets found")
    return 1
