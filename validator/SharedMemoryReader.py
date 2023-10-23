# Shared memory reader module for the main runner class to use.
# contains methods for reading the validation policy and input
# files using the mmap object created using the shared memory.

# imported modules
import SharedMemoryHandler as smh
import InputFileReader as inread
import ValidationPolicyReader as policyread
import Logger as log

def ReadPolicy():
    if(smh.isValidSharedMemory):
        if(smh.isValidMmap):
            policyread.ReadFile(smh.mapFile)
        else:
            log.LogMessage("mmap file is not valid! Has it not been created yet?")
    else:
        log.LogMessage("Shared memory is not valid! Has it not been created yet?")

# Runs the input file reader module to parse and get the data in the specified
# input file data message. This message contains the file name, offset in shared
# memory in bytes, and the size of the input data in bytes
# return the data in the input file, or None if the mmap mapped
# to shared memory isn't valid.
def ReadInputFile(inputFileData):
    if(smh.isValidSharedMemory):
        if(smh.isValidMmap):
            dataToValidate = inread.ReadFile(inputFileData, smh.mapFile)
            return dataToValidate
        else:
            log.LogMessage("mmap file is not valid! Has it not been created yet?")
            return None
    else:
        log.LogMessage("Shared memory is not valid! Has it not been created yet?")
        return None