# updated logger class that sets the configuration
# 2 logs are created to help debug:
# 1. Logs_debug - this is a running log of all levels to debug any issues
# 2. logs_error - this only logs logs using .error() or .critical()

import logging
import logging.config

# to set this up in the modules use the format:
# import Logger as log
# my_logger = log.MyLogger()
# logger = my_logger.getLogger('valLogger')


class Logger:
    def __init__(self):
        self.log_config = {
            'version': 1,
            'formatters': {
                'genFormat': {
                    'format': '%(asctime)s [%(levelname)s] %(name)s: %(message)s',
                    'datefmt': '%Y-%m-%d %H:%M:%S'
                },
                'critFormat': {
                    'format': '%(asctime)s [%(levelname)s] %(name)s: %(message)s',
                    'datefmt': '%Y-%m-%d %H:%M:%S'
                }
            },
            'handlers': {
                'consoleHandler': {
                    'class': 'logging.StreamHandler',
                    'level': 'INFO',
                    'formatter': 'genFormat',
                },
                'debugFileHandler': {
                    'class': 'logging.FileHandler',
                    'level': 'DEBUG',
                    'formatter': 'genFormat',
                    'filename': 'logs_debug.log',
                    'mode': 'a',
                },
                'errorFileHandler': {
                    'class': 'logging.FileHandler',
                    'level': 'ERROR',
                    'formatter': 'genFormat',
                    'filename': 'logs_error.log',
                    'mode': 'a',
                },
            },
            'loggers': {
                'valLogger': {
                    'level': 'DEBUG',
                    'handlers': ['consoleHandler', 'debugFileHandler', 'errorFileHandler'],
                    'propagate': 0,
                }
            }
        }
        logging.config.dictConfig(self.log_config)

    def getLogger(self, name):
        return logging.getLogger(name)
