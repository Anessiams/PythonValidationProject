import InputHandler as inq
import OutputHandler as outq

# This module will be used by the handler to close the IPC mechanasims
# on the validator side so they can be properly unlinked when
# The handler has finished executing
# Note: This module should be called by the handler before unlinking
# the IPC mechanisms because IPC mechanisms will only be
# destroyed by unlinking once all references are closed

# Close message queues and set their isValidQueue boolean to false
def CloseQueues():
    inq.CleanupQueue()
    outq.CleanupQueue()