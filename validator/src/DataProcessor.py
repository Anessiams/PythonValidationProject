import Logger as log

# Test for processing a message from a message queue.
# Calculates the length of the message and sends the
# length of the message to the output message queue.
# if the message we're processing is not a string,
# it sends an error and returns -1 for the length.
def ProcessMessage(message):
    try:
        log.LogMessage("Checking the length of the message.")
        length = len(message)
        return length
    except TypeError:
        log.LogMessage("Error: message is not a string!")
        return -1