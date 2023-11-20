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
