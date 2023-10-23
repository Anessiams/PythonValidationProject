# Libraries
import signal
import atexit

# Internal modules
import InputHandler as inq
import OutputHandler as outq
import SharedMemoryHandler as smh
import SharedMemoryReader as reader
import DataProcessor as processor
import Logger as log
import Cleanup

# Cleanup functions
def ProgramCleanup():
    Cleanup.CloseQueues()
    Cleanup.CloseSharedMemory()

# set up cleanup function
atexit.register(ProgramCleanup)
signal.signal(signal.SIGTERM, ProgramCleanup)
signal.signal(signal.SIGINT, ProgramCleanup)

# Create the shared memory
smh.InitializeSharedMemory()

# Read the data validation policy from the shared memory
# if we failed to create the shared memory IPC, we exit the program
if (smh.isValidSharedMemory):
    if(smh.isValidMmap):
        pass
    else:
        log.LogMessage("mmap of shared memory is invalid! Was it not created?")
        exit(1)
else:
    log.LogMessage("Shared memory is invalid! Was it not created?")
    exit(1)

# Create message queues
inq.InitializeQueue()
outq.InitializeQueue()


# Read message from handler, process message, and send result back
# to the handler.
while(inq.isValidQueue and outq.isValidQueue):
    inputMessage = inq.ReadMessageQueue()
    # inputMessage should only be None if we're ending the validator.
    if(inputMessage != None):
        dataToValidate = reader.ReadInputFile(inputMessage)
        if(dataToValidate == None):
            exit(1)
        else:
            result = processor.ProcessMessage(dataToValidate)
            outq.SendMessage(str(result))
exit(0)