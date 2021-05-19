import os
from threading import Thread

from interface.app import start_browser
from engine.events.handler import engine_event_handler
from logger.logger import logger
from helper.helper import get_main_directory, restart, hide_console, is_admin, message_box, Debug
from helper.config import config
from server import connection

os.chdir(get_main_directory())


def shutdown(*args):
    if len(args) == 2:
        route, websockets = args
        if websockets:
            return

    engine_event_handler.quit()
    connection.close()
    exit(0)


def main():
    try:
        hide_console()

        if not Debug and not is_admin():
            try:
                from elevate import elevate
                elevate(graphical=False, show_console=True)
                return
            except Exception:
                result = message_box("Some features may not be available or may not work properly.\nPlease, run as administrator!", "Warning")
                if result == 4:
                    restart()
                    return
                if result == 5:
                    pass
                else:
                    exit(1)
                    return

        logger.init()
        config.init()

        browser = Thread(target=lambda: start_browser(shutdown))

        browser.start()

        shutdown()

    except KeyboardInterrupt:
        shutdown()


if __name__ == "__main__":
    main()
