import eel

from engine.events.handler import engine_event_handler
from engine.fishing.event import FishEvent
from helper.config import config
from helper.helper import Debug
from logger.logger import LogMessageType
from server import connection

eel.init(".\\interface\\web")


@eel.expose
def is_debug():
    return Debug


@eel.expose
def py_init_connection(host, port, https=True, verify=True):
    return connection.init(host, port, https, verify)


@eel.expose
def py_bot_is_started(start=None):
    return engine_event_handler.is_started(start)


@eel.expose
def py_get_host():
    return config.get("host")


@eel.expose
def py_set_host(host):
    return config.set("host", host)


@eel.expose
def py_get_port():
    return config.get("port")


@eel.expose
def py_set_port(port):
    return config.set("port", port)


@eel.expose
def py_get_https():
    return config.get("https", 1)


@eel.expose
def py_set_https(https):
    return config.set("https", https)


@eel.expose
def py_get_user_login():
    return config.get("login")


@eel.expose
def py_user_login(login, password, eula=None):
    if login is None:
        login = config.get("login")

    status = connection.authorize(login, password, eula)
    if type(status) is str:
        config.set("login", login)
        return login, status

    return status


@eel.expose
def py_get_action_key():
    return config.get("action_key", FishEvent.action_key)


@eel.expose
def py_get_collect_key():
    return config.get("collect_key", FishEvent.collect_key)


@eel.expose
def py_set_action_key(key: str):
    if not key or len(key) != 1:
        raise TypeError

    key = key.lower()
    config.set("action_key", key)
    FishEvent.action_key = key


@eel.expose
def py_set_collect_key(key: str):
    if not key or len(key) != 1:
        raise TypeError

    key = key.lower()
    config.set("collect_key", key)
    FishEvent.collect_key = key


@eel.expose
def py_get_records(start=0, end=0x7fffffff, take=10, skip=0):
    return connection.get_records(start, end, take, skip)


@eel.expose
def py_get_user_records(start=0, end=0x7fffffff, take=10, skip=0):
    return connection.get_user_records(start, end, take, skip)


@eel.expose
def py_get_statistics(start=0, end=0x7fffffff, take=10, skip=0):
    return connection.get_user_statistics(start, end, take, skip)


def js_notify(text, color, delay):
    return eel.notify(text, color, delay)


def js_log_notify(message, msg_type=LogMessageType.Info):
    return eel.log_notify(message, msg_type)


def start_browser(event):
    eel.start("index.html", port=60442, cmdline_args=[], close_callback=event if not Debug else None)
