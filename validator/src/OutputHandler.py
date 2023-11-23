# IPC Library
import posix_ipc as ipc
import Logger as log

# Message queue descriptor and queue name
senderMQ = None
QUEUE_NAME = "/validatorresults"

isValidQueue = False

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')

# Initializes the output message queue, and sets the isValidQueue
# boolean if successful, sends an error message if the message queue
# is not found


def InitializeQueue():
    global isValidQueue
    logger.info('Starting Process: OH InitializeQueue')
    try:
        global senderMQ
        senderMQ = ipc.MessageQueue(QUEUE_NAME, read=False, write=True)
        logger.info('Process OH InitializeQueue Successful')

        isValidQueue = True
        logger.debug(f'isValidQueue set to {isValidQueue}')

    except ipc.ExistentialError:
        logger.error(
            'Output message Queue not found! Has it not been made yet?')
        isValidQueue = False
        logger.debug(f'isValidQueue set to {isValidQueue}')

# No need to check if queue is valid as the queue being valid is a
# prerequisite for calling this method.


def SendMessage(msg):
    logger.info('Starting Process: OH SendMessage')
    if (isValidQueue):
        logger.info(f'Sending message: {str(msg)}')
        senderMQ.send(msg)
        logger.info('Process OH SendMessage Successful, message sent')
    else:
        logger.error("Message not sent: Message queue is not valid!")

# This function will be called by the cleanup module once the handler
# has finished executing.


def CleanupQueue():
    global isValidQueue
    logger.info('Starting Process: OH CleanupQueue ')
    try:
        senderMQ.close()
        logger.info('Process OH CleanupQueue Successful, queue closed')
    except:
        logger.debug('No message queue found to close')
    finally:
        logger.debug('The queue is not valid, unable to clean up')
        isValidQueue = False
        logger.debug(f'isValidQueue set to {isValidQueue}')
