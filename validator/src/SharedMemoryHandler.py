# IPC Library
import posix_ipc as ipc

# Other libraries
import mmap as memorymap

# Imported modules
import Logger as log

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')

# This name should be changed to match the name of the shared memory
# as defined by the handler
SHARED_MEMORY_NAME = "file-data"
sharedMemory = None
isValidSharedMemory = False

mapFile = None
isValidMmap = False


def InitializeSharedMemory():
    # Create the shared memory IPC mechanism
    logger.info('Staring Process: SMH InitializeSharedMemory')
    global isValidSharedMemory

    try:
        global sharedMemory
        sharedMemory = ipc.SharedMemory(SHARED_MEMORY_NAME, read_only=True)
        logger.debug('Process SMH InitializeSharedMemory Successful')

        isValidSharedMemory = True
        logger.debug(f'isSharedMemory set to{isValidSharedMemory}')
    except ipc.ExistentialError:
        logger.error('IPC Existential Error: Shared message IPC not found! Has it not been created yet?')
        isValidSharedMemory = False
        logger.debug(f'isValidSharedMemory set to{isValidSharedMemory}')

    if isValidSharedMemory:
        # Mmap the shared memory
        global isValidMmap
        global mapFile
        logger.debug('Staring mmap to shared memory')
        try:
            mapFile = memorymap.mmap(
                sharedMemory.fd, sharedMemory.size, prot=memorymap.PROT_READ)
            logger.debug('Mmap process was successful')

            isValidMmap = True
            logger.debug(f'isValidMmap set to{isValidMmap}')
        except Exception as e:
            print(e)
            logger.debug(f'Mmap process failed - {e}')

            isValidMmap = False
            logger.debug(f'isValidMmap set to{isValidMmap}')
    else:
        logger.error('Shared memory is not valid!')


def CleanupSharedMemory():
    global isValidMmap
    logger.info('Starting Process: SMH CleanupSharedMemory')
    try:
        mapFile.close()
        logger.info('Process SMH CleanupSharedMemory successful, mmap file closed')
    except:
        log.debug('No Mmap file to clean up')
    finally:
        isValidMmap = False
        log.error(f'Mmap file is not valid, unable to clean up. isValidMmap set to{isValidMmap}')
    global isValidSharedMemory
    try:
        sharedMemory.close_fd()
        logger.info('Process SMH CleanupSharedMemory successful, shared memory closed')
    except:
        log.debug('No shared memory to clean up')
    finally:
        isValidSharedMemory = False
        log.error(f'Mmap file is not valid, unable to clean up. isValidMmap set to{isValidMmap}')
