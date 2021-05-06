import ctypes
import os
import sys
import threading
import time
import traceback
from logger.logger import logger, LogMessageType

__main_dir__ = None


def get_main_dir():
    global __main_dir__

    if __main_dir__ is None:
        abspath = os.path.abspath(__file__)
        __main_dir__ = os.path.abspath(os.path.join(os.path.dirname(abspath), os.pardir))

    return __main_dir__


def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except Exception:
        return False


def sort_dict(dictionary: dict):
    return {key: dictionary[key] for key in sorted(dictionary.keys())}


def not_implemented():
    logger.log("Not Implemented", LogMessageType.Error)


def empty_function():
    pass


def wait_until(func):
    while not func():
        time.sleep(0.1)


def sign(x):
    return -1 if x < 0 else 1


def install_thread_excepthook():
    run_old = threading.Thread.run

    # noinspection PyBroadException
    def run(*args, **kwargs):
        try:
            run_old(*args, **kwargs)
        except (KeyboardInterrupt, SystemExit):
            raise
        except Exception:
            sys.excepthook(*sys.exc_info())

    threading.Thread.run = run


def unhandled_exception_logging(*exc_info):
    text = "".join(traceback.format_exception(*exc_info))
    logger.log(f"Unhandled exception: {text}", LogMessageType.Error)


def restart():
    os.execl(sys.executable, *([sys.executable] + sys.argv))
