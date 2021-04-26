import ctypes
import os
from threading import Thread

from elevate import elevate

from interface.app import start_browser
from engine.event_handler import engine_event_handler
from logger.logger import logger
from helper.helper import get_main_dir
from helper.config import config
from server import connection

os.chdir(get_main_dir())


def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except Exception:
        return False


def quit_app(*args):
    if len(args) == 2:
        route, websockets = args
        if websockets:
            return

    engine_event_handler.quit()
    connection.close()
    config.stop()
    exit(0)


def main():
    try:
        ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)

        if not is_admin():
            try:
                elevate(graphical=False, show_console=True)
                return
            except Exception:
                print("Please, run as administrator!")
                exit(1)

        logger.init()
        config.init()

        browser = Thread(target=lambda: start_browser(quit_app))

        browser.start()

        quit_app()

    except KeyboardInterrupt:
        quit_app()


if __name__ == "__main__":
    main()
