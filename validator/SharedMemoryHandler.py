# IPC Library
import posix_ipc as ipc

# Other libraries
import mmap

# Imported modules
import Logger as log

# This name should be changed to match the name of the shared memory
# as defined by the handler
SHARED_MEMORY_NAME = "ValidatorMemory"
sharedMemory = None
isValidSharedMemory = False
mapFile = None
isValidMmap = False

def InitializeSharedMemory():
    # Create the shared memory IPC mechanism
    global isValidSharedMemory
    try:
        global sharedMemory
        sharedMemory = ipc.SharedMemory(SHARED_MEMORY_NAME)
        isValidSharedMemory = True
    except ipc.ExistentialError:
        log.LogMessage("Shared message IPC not found! Has it not been created yet?")
        isValidSharedMemory = False
    if isValidSharedMemory:
        # Mmap the shared memory
        global isValidMmap
        global mapFile
        mapFile = mmap.mmap(sharedMemory.fd, sharedMemory.size)
        isValidMmap = True

def CleanupSharedMemory():
    global isValidMmap
    try:
        mapFile.close()
        log.LogMessage("Mmap file closed")
    except:
        log.LogMessage("No need to cleanup Mmap file")
    finally:
        isValidMmap = False
    global isValidSharedMemory
    try:
        sharedMemory.close_fd()
        log.LogMessage("Shared memory closed")
    except:
        log.LogMessage("No need to cleanup shared memory file")
    finally:
        isValidSharedMemory = False