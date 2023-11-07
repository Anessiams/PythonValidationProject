# validator


import multiprocessing.shared_memory


# Open the data file to read its contents
data_file_path = 'hello2_1.txt'
with open(data_file_path, 'r') as data_file:
    data_file_contents = data_file.read()

# Encode the data file as bytes
data_file_bytes = data_file_contents.encode('utf-8')

# Create a shared memory region for the data file
shm_data_file = multiprocessing.shared_memory.SharedMemory(
    create=True, size=len(data_file_bytes)
)

# Write data file data to shared memory
shm_data_file.buf[:len(data_file_bytes)] = data_file_bytes

# Open the policy file to read its contents
example_policy_path = 'hello2.py'
with open(example_policy_path, 'rb') as policy_file:
    policy_file_contents = policy_file.read()

# Create a shared memory region for the policy file data
shm_policy_file = multiprocessing.shared_memory.SharedMemory(
    create=True, size=len(policy_file_contents)
)

# Write policy file data to shared memory
shm_policy_file.buf[:len(policy_file_contents)] = policy_file_contents

'''
def validate(policy, data):
    #policy = example_policy_path
    #data = data_file_path
    try:
        py
    except Exception as e:
        print('Fail')
'''



# Execute the modified policy script with the data file as an argument
try:
    exec(f'import sys; sys.argv = ["hello2.py", "hello2_1.txt"]; exec(shm_policy_file.buf[:].tobytes())')
except Exception as e:
    print(f"Error executing code: {e}")

# Close and unlink the shared memory when done
shm_data_file.close()
shm_data_file.unlink()
shm_policy_file.close()
shm_policy_file.unlink()
