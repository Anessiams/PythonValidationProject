# IPC Library
import posix_ipc as ipc

# Imported modules
import Logger as log

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')

# Message Queue descriptor.
receiverMQ = None
QUEUE_NAME = "/filesunvalidated"
isValidQueue = False

# Special ASCII characters
NULL_TERMINATOR = chr(0)

# Initializes the input message queue, and sets the isValidQueue
# boolean if successful, sends an error message if the message queue
# is not found


def InitializeQueue():
    global isValidQueue
    logger.info('Starting Process: IH InitializeQueue')
    try:
        global receiverMQ
        receiverMQ = ipc.MessageQueue(QUEUE_NAME, read=True, write=False)
        isValidQueue = True
        logger.info('Process IH InitializeQueue Successful')
        logger.debug(f'isValidQueue set to {isValidQueue}')
    except ipc.ExistentialError:
        logger.error(
            'Input message Queue not found! Has it not been made yet?')
        isValidQueue = False
        logger.debug(f'isValidQueue set to {isValidQueue}')

# No need to check if queue is valid as the queue being valid is a
# prerequisite for calling this method.
# If the process is killed by a signal, we make the queue not valid
# so the runner program can end properly


def ReadMessageQueue():
    global isValidQueue
    logger.info('Starting Process: IH ReadMessageQueue')

    try:
        if (isValidQueue):
            # receive method returns a tuple of message and the priority
            # we store priority in a variable to make sure msg
            # is stored as bytes
            msg, priority = receiverMQ.receive()
        # First we decods msg from bytes into a string
        # Second, to get the proper string length, we split the msg
    # into two parts, the string itself, and the null terminator
    # character used in C strings
    # Finally, we grab the first message, which is the full string
    # minus the null terminator
            msg = msg.decode().split(NULL_TERMINATOR)
            msg = msg[0]
            logger.info(f'Process ReadMessageQueue Successful, 
                        Received message: {str(msg)}')
            return str(msg)
        else:
            logger.error('Message queue is invalid!')
            return None
    except ipc.SignalError:
        logger.error('Message queue Invalid- Encountered IPC Signal Error')
        isValidQueue = False
        logger.debug(f'isValidQueue set to {isValidQueue}')
        return None

# This function will be called by the cleanup module once the handler
# has finished executing.


def CleanupQueue():
    global isValidQueue
    logger.info('Starting Process: IH CleanupQueue')
    try:
        receiverMQ.close()
        logger.info('Process IH CleanupQueue Successful, queue closed')
    except:
        log.debug('No message queue found to close')
    finally:
        log.error('The queue is not valid, unable to clean up')
        isValidQueue = False
        logger.debug(f'isValidQueue set to {isValidQueue}')
