# updated logger class that sets the configuration
# 2 logs are created to help debug:
# 1. Logs_debug - this is a running log of all levels to debug any issues
# 2. logs_error - this only logs logs using .error() or .critical()

import logging
import logging.config
import os

# to set this up in the modules use the format:
# import Logger as log
# logger = log.Logger()
# logger = logger.getLogger('valLogger')


class Logger:
    def __init__(self):

        # Get the current working directory and create a log dir if one does not exist
        current_dir = os.getcwd()
        logs_dir = os.path.join(current_dir, 'logs')
        os.makedirs(logs_dir, exist_ok=True)

        self.log_config = {
            'version': 1,
            'formatters': {
                'genFormat': {
                    'format': '%(asctime)s [%(levelname)s] %(name)s: %(filename)s - %(message)s',
                    'datefmt': '%Y-%m-%d %H:%M:%S'
                },
                'critFormat': {
                    'format': '%(asctime)s [%(levelname)s] %(name)s: %(filename)s - %(message)s',
                    'datefmt': '%Y-%m-%d %H:%M:%S'
                }
            },
            'handlers': {
                'consoleHandler': {
                    'class': 'logging.StreamHandler',
                    'level': logging.INFO,
                    'formatter': 'genFormat',
                },
                'debugFileHandler': {
                    'class': 'logging.FileHandler',
                    'level': logging.DEBUG,
                    'formatter': 'genFormat',
                    'filename': os.path.join(logs_dir, 'logs_debug.log'),
                    'mode': 'a',
                },
                'errorFileHandler': {
                    'class': 'logging.FileHandler',
                    'level': logging.ERROR,
                    'formatter': 'genFormat',
                    'filename': os.path.join(logs_dir, 'logs_error.log'),
                    'mode': 'a',
                },
            },
            'loggers': {
                'valLogger': {
                    'level': logging.DEBUG,
                    'handlers': ['consoleHandler', 'debugFileHandler', 'errorFileHandler'],
                    'propagate': 0,
                }
            }
        }
        logging.config.dictConfig(self.log_config)

    def getLogger(self, name):
        return logging.getLogger(name)
