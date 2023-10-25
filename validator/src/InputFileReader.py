# File reader module to read the input files from shared memory

# imported modules
import Logger as log

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')

# Special ASCII Character
UNIT_SEPERATOR = chr(31)

# Reads the input data file using the shared memory's mmap.
# Reads the file byte by byte, and parses the necessary data to send
# to the validator module
# returns the data it read to be used by the validator module?


def ReadFile(inputFile, mappedFile):
    logger.info('Starting Process: ReadFile')
    inputFileTokens = ParseInputMessage(inputFile)
    if (inputFileTokens == None):
        logger.debug('inputFileTokens array empty')
        return None
    else:
        logger.info(f'Reading input file: {inputFileTokens[0]}')

        offset = int(inputFileTokens[1])
        logger.debug(f'Offset value: {offset}')

        # Seek the mmap based on the input file's offset
        mappedFile.seek(offset)

        # Get the file size in bytes as an integer
        fileSize = int(inputFileTokens[2])
        logger.debug(f'file size: {fileSize}')

        # Create the byte array holding all the bytes read from the file
        dataToValidate = []

        # read size number of bytes
        logger.debug('Reading Bytes from mmap and appending to dataToValidate')
        for i in range(fileSize):
            c = mappedFile.read_byte()  # Get the current byte the mmap is pointing to
            logger.debug(f'mmap pointing to byte {c}')
            # Append the byte we read to the variable holding all the bytes we've read
            dataToValidate.append(c)
            logger.debug(f'Appended dataToValidate: {dataToValidate}')
            

        # Convert the byte array into a string using join with an empty character as the seperator.
        dataToValidate = ''.join([chr(c) for c in dataToValidate])
        logger.debug(f'Final byte array: {dataToValidate}')
        logger.info(f'Process ReadFile Successful, 
                    Sending data in {inputFileTokens[0]} to the validator')
        inputFileAndData = [inputFileTokens[0], dataToValidate]
        return inputFileAndData


# Parse the input message into the input file name, offset, and size
# Returns a string array splitting the input message into the three
# section as defined:
# 0 - Input file name
# 1 - file offset (in bytes)
# 2 - file length (in bytes)
def ParseInputMessage(messageToParse):
    logger.info('Starting Process: ParseInputMessage')
    # split our input message into the three sections seperated by the unit seperator character
    parsedMessage = messageToParse.split(UNIT_SEPERATOR)

    # if we split the string into more than three elements, it means that we encountered too many seperators
    if (len(parsedMessage) != 3):
        logger.error(f'invalid format- Expected: 3, Received: {parsedMessage.len}')
        parsedMessage = None

    logger.info(f'Process ParseInputMessage Successful, sending {parsedMessage}')
    return parsedMessage
