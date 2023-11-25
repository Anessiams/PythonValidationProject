# File reader responsible for reading the data validation policy
# from the shared memory IPC mechanism

# imported modules
import Logger as log
import struct

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')

# Read the file, using the given mmap object, and process the data
# of the validation policy file accordingly.
def ReadFile(mappedFile):
    logger.info("Starting process: ParsePolicyFile")
    mappedFile.seek(0)  # set mmap to beginning of shared memory

    # First 8 bytes: Number of policy files (unused)
    mappedFile.read(8)

    # Next 4096 bytes: Name of the policy file to read
    policyFileName = mappedFile.read(4096).decode('utf-8', 'ignore').strip()
    logger.info(f'Parsing policy file: {policyFileName}')

    # Next 8 bytes: offset of the policy file to read
    pfoBytes = mappedFile.read(8)
    policyFileOffset = struct.unpack('<Q', pfoBytes)[0]
    logger.info(f'Policy file offset (bytes): {policyFileOffset}')

    # Next 8 bytes: size of the policy file to read
    pfsBytes = mappedFile.read(8)
    policyFileSize = struct.unpack('<Q', pfsBytes)[0]
    logger.info(f'Policy file size (bytes): {policyFileSize}')

    # Read the policy file data
    logger.debug('Reading Bytes from mmap and storing it in policyFileData')
    mappedFile.seek(policyFileOffset)
    policyFileData = mappedFile.read(policyFileSize).decode()

    logger.debug(f'Final string: {policyFileData}')
    logger.info(
        f'Process ParsePolicyFile successful. Input data will be checked against the {policyFileName} file.')

    policyFileNameAndData = [policyFileName, policyFileData]
    return policyFileNameAndData
