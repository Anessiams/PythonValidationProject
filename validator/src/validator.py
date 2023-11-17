import mmap
import struct

# shared memory block path and metadata format
SHARED_MEMORY_PATH = '/dev/shm/my_shared_memory' # arbitrary shm path
METADATA_FORMAT = '4096sqq'
METADATA_OFFSET = 8  # Skips first 8 bytes

def execute_script(policy_file, data_file):
    try:
        '''
        This function creates an empty dictionary that will store the 
        variables and functions defined by the policy file. It also allows
        us to execute any python script stored in shared memory and interact with 
        them by calling their functions and passing data from shared memeory. 
        In this function we assume that the policy file has a function called 
        validate and we retrive that function to later pass in data as it's argument.
        '''
        # This dictionary serves as the container for the exec() call
        policy_dict = {}

        # Executes the policy file. Stores potential global var or functions in {}, 
        # and local functions and variables in policy_dict 
        exec(policy_file, {}, policy_dict)

        # Provides a reference to the validate function defined by the policy file
        validate_func = policy_dict['validate']

        # Error checks on the return value of the validate function
        if validate_func is None or not callable(validate_func):
            raise ValueError("The policy file must contain a function named 'validate'.")

        # Calls the validate function in the with data_file as an argument and returns its output
        return validate_func(data_file)
    
    except Exception as e:
        return f"Exception: {str(e)}"

'''
The open() function in this context is opening a file that represents
a block of shared memory. While this file is in the file system, it's 
not a regular file that is stored in disk. Instead it is stored in memory. 
So when you read/write from or to the file, you're doing this to a block of memory. 
'''
try:

    with open(SHARED_MEMORY_PATH, 'r+b') as f:
        mm = mmap.mmap(f.fileno(), 0)

    # Skips the first METADATA_OFFSET bytes and then unpack metadata from shared memory block
    name, offset, size = struct.unpack(METADATA_FORMAT, mm[METADATA_OFFSET:METADATA_OFFSET + struct.calcsize(METADATA_FORMAT)])

    # Converts the "name" from bytes to string and strip null characters
    name = name.decode().rstrip('\x00')

    # Read python script and data file from shared memory
    policy_file = mm[offset:offset+size].decode()
    data_file = mm[offset+size:offset+2*size].decode()

    result = execute_script(policy_file, data_file)
    print(result)  # Prints the result returned by validate function

except FileNotFoundError:
    print(f"Error: The file at {SHARED_MEMORY_PATH} was not found.")

except Exception as e:
    print(f"Exception: {str(e)}")

