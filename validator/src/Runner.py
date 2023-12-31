# Libraries
import signal
import atexit

# Internal modules
import InputHandler as inq
import OutputHandler as outq
import SharedMemoryHandler as smh
import SharedMemoryReader as reader
import validator as val
import Logger as log
import Cleanup

# module variables
policyFileNameData = None

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')

# Cleanup functions


def ProgramCleanup():
    logger.info('Starting Process: ProgramCleanup')
    Cleanup.CloseQueues()
    logger.debug('Success: Queues closed')
    Cleanup.CloseSharedMemory()
    logger.debug('Success: Shared Memory connection closed')


# set up cleanup function
atexit.register(ProgramCleanup)
signal.signal(signal.SIGTERM, ProgramCleanup)
signal.signal(signal.SIGINT, ProgramCleanup)

# Create the shared memory
smh.InitializeSharedMemory()
logger.debug('shared memory created')

# Read the data validation policy from the shared memory
# if we failed to create the shared memory IPC, we exit the program
logger.info('Starting Process: Reading data validation policy')
if (smh.isValidSharedMemory):
    logger.debug('Shared Memory is valid')
    if (smh.isValidMmap):
        logger.debug('mmap is valid')
        # First item is policy file name, second item is the data itself.
        policyFileNameData = reader.ReadPolicy()
        if (policyFileNameData == None):
            logger.error(
                'Reading and parsing policy file failed. Exiting program')
            exit(1)
    else:
        logger.error(
            'mmap of shared memory invalid, check if it was created. Exititing program')
        exit(1)
else:
    logger.error(
        'Shared memory invalid, check if it was created. Exititing program')
    exit(1)

# Create message queues
logger.info('Starting Process: Message Queue Creation')
inq.InitializeQueue()
logger.debug('Success: Input message queues created')
outq.InitializeQueue()
logger.debug('Success: Output message queues created')


# Read message from handler, process message, and send result back
# to the handler.
logger.info('Starting Process: reading, processing, returning handler message')

while (inq.isValidQueue and outq.isValidQueue):
    inputMessage = inq.ReadMessageQueue()
    logger.debug(f'validating input message: {inputMessage}')
    # inputMessage should only be None if we're ending the validator.
    if (inputMessage != None):
        dataToValidate = reader.ReadInputFile(inputMessage)
        if (dataToValidate == None):
            logger.info('Ending Validation process')
            exit(1)
        else:
            logger.debug(
                f'Validating data file {dataToValidate[1]} against policy file {policyFileNameData[1]}')
            result = val.execute_script(
                policyFileNameData[1], dataToValidate[1])
            logger.debug(f'Validator result: {result}')
            outq.SendMessage(dataToValidate[0] + chr(31) + str(result))
            logger.debug('message sent to output')

logger.info('Ending Program')
exit(0)
