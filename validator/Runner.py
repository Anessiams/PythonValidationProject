# Libraries
import signal
import atexit

# Internal modules
import InputHandler as inq
import OutputHandler as outq
import DataProcessor as processor
import Cleanup

# Cleanup functions
def ProgramCleanup():
    Cleanup.CloseQueues()

# set up cleanup function
atexit.register(ProgramCleanup)
signal.signal(signal.SIGTERM, ProgramCleanup)
signal.signal(signal.SIGINT, ProgramCleanup)

# Create message queues
inq.InitializeQueue()
outq.InitializeQueue()


# Read message from handler, process message, and send result back
# to the handler.
while(inq.isValidQueue and outq.isValidQueue):
    inputMessage = inq.ReadMessageQueue()
    # inputMessage should only be None if we're ending the validator.
    if(inputMessage != None):
        output = str(processor.ProcessMessage(inputMessage))
        outq.SendMessage(output)
exit(0)
