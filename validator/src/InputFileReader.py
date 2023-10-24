# File reader module to read the input files from shared memory

# imported modules
import Logger as log

# Special ASCII Character
UNIT_SEPERATOR = chr(31)

# Reads the input data file using the shared memory's mmap.
# Reads the file byte by byte, and parses the necessary data to send
# to the validator module
# returns the data it read to be used by the validator module?
def ReadFile(inputFile, mappedFile):
    inputFileTokens = ParseInputMessage(inputFile)
    if(inputFileTokens == None):
        return None
    else:
        log.LogMessage("Reading input file: " + inputFileTokens[0])
        offset = int(inputFileTokens[1])
        # Seek the mmap based on the input file's offset
        mappedFile.Seek(offset)
        # Get the file size in bytes as an integer
        fileSize = int(inputFileTokens[2])
        # Create the byte array holding all the bytes read from the file
        dataToValidate = []
        # read size number of bytes
        for i in range(fileSize):
            c = mappedFile.read_byte() # Get the current byte the mmap is pointing to
            dataToValidate.append(c) # Append the byte we read to the variable holding all the bytes we've read
        # Convert the byte array into a string using join with an empty character as the seperator.
        dataToValidate = ''.join([chr(c) for c in dataToValidate])
        log.LogMessage("Sending data in " + inputFileTokens[0] + " to the validator")
        return dataToValidate
        

# Parse the input message into the input file name, offset, and size
# Returns a string array splitting the input message into the three
# section as defined:
# 0 - Input file name
# 1 - file offset (in bytes)
# 2 - file length (in bytes)
def ParseInputMessage(messageToParse):
    # split our input message into the three sections seperated by the unit seperator character
    parsedMessage = messageToParse.split(UNIT_SEPERATOR)
    # if we split the string into more than three elements, it means that we encountered too many seperators
    if(len(parsedMessage) != 3):
        log.LogMessage("Input message was in an invalid format, expected 3 tokens, got " + parsedMessage.len() + " instead.")
        parsedMessage = None
    return parsedMessage