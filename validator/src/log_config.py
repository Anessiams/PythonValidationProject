# to use this configuration file use the format: 
# import logging.config
# from log_config import log_config
# logging.config.dictConfig(log_config)
# logger = logging.getLogger('valLogger')

import os
import time

# Create directory files to manage logs
date = time.strftime("%m-%Y")
logDir = os.path.join('logs', date)

# adding logfiles for critical errors
cLogFile = os.path.join(logDir, 'critical.log')
eLogFile = os.path.join(logDir, 'error.log')

# Placeholder to add logging in two files (maybe)

log_config = {
    'version': 1,
    'formatters': {
        'genFormat': {
            'format': '%(asctime)s [%(levelname)s] %(name)s: %(message)s',
            'datefmt': '%Y-%m-%d %H:%M:%S'
        },
        'critFormat': {
            #tbd
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
            'filename': 'validator.log',
            'mode': 'a',
        },
        'errorFileHandler': {
            'class': 'logging.FileHandler',
            'level': 'ERROR',
            'formatter': 'Formatter',
            'filename': eLogFile,
            'mode': 'a',
        },
        'criticalFileHandler': {
            'class': 'logging.FileHandler',
            'level': 'CRITICAL',
            'formatter': 'critFomat',
            'filename': cLogFile,
            'mode': 'a',
        },
    },
    'root': {
        'level': 'DEBUG',
        'handlers': ['consoleHandler', 'debugFileHandler', 'errorFileHandler', 'criticalFileHandler'],
    },
    'loggers': {
        'valLogger': {
            'level': 'DEBUG',
            'handlers': ['consoleHandler','debugFileHandler', 'errorFileHandler', 'criticalFileHandler'],
            'propagate': 0,
        }
    }
}
