import os
from threading import Thread

from interface.app import start_browser
from engine.events.handler import engine_event_handler
from logger.logger import logger
from helper.helper import get_main_dir, restart
from helper.config import config
from server import connection

os.chdir(get_main_dir())


def is_admin():
    try:
        from ctypes import windll
        admin = windll.shell32.IsUserAnAdmin()
        del windll

        return admin
    except Exception:
        return False


def msgbox(message, title):
    try:
        from ctypes import windll
        value = windll.user32.MessageBoxW(0, message, title, 50)
        del windll

        return value
    except Exception:
        print(f"{title}: {message}")
        return None


def hide_console():
    try:
        from ctypes import windll
        windll.user32.ShowWindow(windll.kernel32.GetConsoleWindow(), 0)
        del windll

        return True
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
        hide_console()

        if not is_admin():
            try:
                from elevate import elevate
                elevate(graphical=False, show_console=True)
                return
            except Exception:
                result = msgbox("Some features may not be available or may not work properly.\nPlease, run as administrator!", "Warning")
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

        browser = Thread(target=lambda: start_browser(quit_app))

        browser.start()

        quit_app()

    except KeyboardInterrupt:
        quit_app()


if __name__ == "__main__":
    main()
