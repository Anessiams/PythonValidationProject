# IPC Library
import posix_ipc as ipc
import Logger as log

# Message Queue descriptor.
receiverMQ = None
QUEUE_NAME = "/filesunvalidated"
isValidQueue = False

# Note: any print methods in this should be changed into a function
# call to the logger module

# Initializes the input message queue, and sets the isValidQueue
# boolean if successful, sends an error message if the message queue
# is not found
def InitializeQueue():
    global isValidQueue
    try:
        global receiverMQ
        receiverMQ = ipc.MessageQueue(QUEUE_NAME, read = True, write = False)
        isValidQueue = True
    except ipc.ExistentialError:
        log.LogMessage("Input message Queue not found! Has it not been made yet?")
        isValidQueue = False

# No need to check if queue is valid as the queue being valid is a
# prerequisite for calling this method.
# If the process is killed by a signal, we make the queue not valid
# so the runner program can end properly
def ReadMessageQueue():
    try:
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
        msg = msg.decode().split('\0')
        msg = msg[0]
        log.LogMessage("Received message: " + msg)
        return msg
    except ipc.SignalError:
        global isValidQueue 
        isValidQueue = False
        return None

# This function will be called by the cleanup module once the handler
# has finished executing.
def CleanupQueue():
    global isValidQueue
    try:
        receiverMQ.close()
        log.LogMessage("Input message queue closed")
    except:
        log.LogMessage("No need to close input message queue")
    finally:
        isValidQueue = False