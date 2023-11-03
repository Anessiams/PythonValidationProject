import Logger as log

# Test for processing a message from a message queue.
# Calculates the length of the message and sends the
# length of the message to the output message queue.
# if the message we're processing is not a string,
# it sends an error and returns -1 for the length.

# Creation and Set-up of the logger
logger = log.Logger()
logger = logger.getLogger('valLogger')


def ProcessMessage(message):
    logger.info('Staring Process: ProcessMessage')
    try:
        length = len(message)
        logger.info(
            f'Process ProcessMessage Successful. Message {message}, Length {length}')
        return length
    except TypeError:
        logger.error("Type Error: message is not a string!")
        return -1
