# Shared memory reader module for the main runner class to use.
# contains methods for reading the validation policy and input
# files using the mmap object created using the shared memory.

# imported modules
import SharedMemoryHandler as smh
import InputFileReader as inread
import ValidationPolicyReader as policyread
import Logger as log

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')


def ReadPolicy():
    logger.info('Starting Process: SMR ReadPolicy')
    if (smh.isValidSharedMemory):
        if (smh.isValidMmap):
            policyread.ReadFile(smh.mapFile)
            logger.info('Process SMR ReadPolicy Successful')
        else:
            logger.error(
                'mmap file is not valid! Has it not been created yet?')
            return None
    else:
        logger.error(
            'Shared memory is not valid! Has it not been created yet?')
        return None

# Runs the input file reader module to parse and get the data in the specified
# input file data message. This message contains the file name, offset in shared
# memory in bytes, and the size of the input data in bytes
# return the data in the input file, or None if the mmap mapped
# to shared memory isn't valid.


def ReadInputFile(inputFileData):
    logger.info('Starting Process: SMR ReadInputFile')
    if (smh.isValidSharedMemory):
        logger.debug('Shared Memory is valid, checking mmap')
        if (smh.isValidMmap):
            logger.debug('mmap is valid')
            dataToValidate = inread.ReadFile(inputFileData, smh.mapFile)
            logger.info('Process SMR ReadInputFile Successful')
            return dataToValidate
        else:
            logger.error(
                'mmap file is not valid! Has it not been created yet?')
            return None
    else:
        logger.error(
            'Shared memory is not valid! Has it not been created yet?')
        return None
