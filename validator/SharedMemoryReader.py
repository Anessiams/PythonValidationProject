# Shared memory reader module for the main runner class to use.
# contains methods for reading the validation policy and input
# files using the mmap object created using the shared memory.

# imported modules
import SharedMemoryHandler as smh
import InputFileReader as inread
import ValidationPolicyReader as policyread

def ReadPolicy():
    policyread.ReadFile(smh.mapFile)

# Runs the input file reader module to parse and get the data in the specified
# input file data message. This message contains the file name, offset in shared
# memory in bytes, and the size of the input data in bytes
# return the data in the input file, or None if the mmap mapped
# to shared memory isn't valid.
def ReadInputFile(inputFileData):
    if(smh.isValidMmap):
        dataToValidate = inread.ReadFile(inputFileData, smh.mapFile)
        return dataToValidate
    else:
        return None