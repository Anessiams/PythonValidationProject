# policy_parser.py

import importlib
from policy import policy # Imports policy dictionary from the 'policy' module
from data import data #Imports data dictionary from the 'data' module

def get_polkeys(file_path):
    policy_keys = []

    try:
        policy_module = importlib.import_module(file_path)
        # Check if the imported module has a 'policy' attribute and is a dictionary
        if hasattr(policy_module, 'policy') and isinstance(policy_module.policy, dict):
            policy_keys = list(policy_module.policy.keys())  # Get the keys from the 'policy' dictionary
        else:
            print("The policy file does not contain a valid 'policy' dictionary.")
    except ImportError:
        print("Failed to import the policy file.")

    return policy_keys

def get_datakeys(file_path):
    data_keys = []

    try:
        data_module = importlib.import_module(file_path)
        # Check if the imported module has a 'data' attribute and is a dictionary
        if hasattr(data_module, 'data') and isinstance(data_module.data, dict):
            data_keys = list(data_module.data.keys())  # Get the keys from the 'data' dictionary
        else:
            print("The policy file does not contain a valid 'policy' dictionary.")
    except ImportError:
        print("Failed to import the policy file.")

    return data_keys


if __name__ == "__main__":
    policy_file = 'policy'  # Change this to the name of the data file without the ".py" extension
    policy_keys = get_polkeys(policy_file)


    data_file = 'data'  # Change this to the name of the policy file without the ".py" extension
    data_keys = get_datakeys(data_file)

    for eachDataKey in data_keys:
        if eachDataKey in policy_keys:
            # Check if the data and policy values for a key have the same type
            if type(data[eachDataKey]) != type(policy[eachDataKey]):
                result = "Fail" # If types don't match, set result to "Fail"
                break
            else:
                 result = "Pass"
    
    print(result)

