import Logger as log

logger = log.Logger()
logger = logger.getLogger('valLogger')


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
        logger.debug(f'Policy dictionary container created')
        # Executes the policy file. Stores potential global var or functions in {},
        # and local functions and variables in policy_dict
        logger.debug(f'Starting execution of policy file {policy_file}')
        exec(policy_file, {}, policy_dict)

        # Provides a reference to the validate function defined by the policy file
        logger.debug(f'Looking for validate function')
        validate_func = policy_dict['validate']

        # Error checks on the return value of the validate function
        if validate_func is None or not callable(validate_func):
            logger.error(f'policy file does not have a validate function')
            raise ValueError(
                "The policy file must contain a function named 'validate'.")

        logger.debug(f'Validate function found')
        logger.debug(f'Getting the validation result')

        # Calls the validate function in the with data_file as an argument and returns its output
        # storing the value in a result variable so it can be logged and returned
        result = validate_func(data_file)

        if result == 0:
            logger.info(f'received {result} : Test has failed')
        else:
            logger.info(f'received {result} : Test has passed')

        logger.debug(f'returning result to runner')
        return result

    except Exception as e:
        logger.error(f'Exception error in validator.py')
        return f"Exception: {str(e)}"
