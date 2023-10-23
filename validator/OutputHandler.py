# IPC Library
import posix_ipc as ipc
import Logger as log

# Message queue descriptor and queue name
senderMQ = None
QUEUE_NAME = "/PythonToCpp"
isValidQueue = False

# Initializes the output message queue, and sets the isValidQueue
# boolean if successful, sends an error message if the message queue
# is not found
def InitializeQueue():
    global isValidQueue
    try:
        global senderMQ
        senderMQ = ipc.MessageQueue(QUEUE_NAME, read = False, write = True)
        isValidQueue = True
    except ipc.ExistentialError:
        log.LogMessage("Output message queue not found! Has it not been made yet?") 
        isValidQueue = False

# No need to check if queue is valid as the queue being valid is a
# prerequisite for calling this method.
def SendMessage(msg):
    if(isValidQueue):
        log.LogMessage("Sending message: " + msg)
        senderMQ.send(msg)
    else:
        log.LogMessage("Message queue is not valid!")

# This function will be called by the cleanup module once the handler
# has finished executing.
def CleanupQueue():
    global isValidQueue
    try:
        senderMQ.close()
        log.LogMessage("Output message queue closed")
    except:
        log.LogMessage("No need to close output message queue")
    finally:
        isValidQueue = False