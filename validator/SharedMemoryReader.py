# Shared memory reader module for the main runner class to use.
# contains methods for reading the validation policy and input
# files using the mmap object created using the shared memory.

# imported modules
import SharedMemoryHandler as smh
import InputFileReader as inread
import ValidationPolicyReader as policyread

def ReadPolicy():
    policyread.ReadFile(smh.mapFile)

def ReadInputFile(inputFileData):
    dataToValidate = inread.ReadFile(inputFileData, smh.mapFile)
    return dataToValidate