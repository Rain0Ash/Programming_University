"""
config.py
Saves configuration in file as json file
"""
import json
import os
from typing import Optional

from event_scheduler import EventScheduler

from .helper import get_main_dir


__config_path__ = _filename = os.path.join(get_main_dir(), "config.json")


def filename():
    return __config_path__


temp_file = os.path.join(os.environ["TEMP"], "esofisher_config.BAK")


class Config:

    def __init__(self):
        self._config_dict: Optional[dict] = None
        self._scheduler: Optional[EventScheduler] = None

    def __getitem__(self, item):
        return self._config_dict.get(item)

    def __setitem__(self, key, value):
        self._config_dict[key] = value

    def __delitem__(self, key):
        del self._config_dict[key]

    def initialize(self):
        self._scheduler = EventScheduler()
        if os.path.exists(filename()):

            try:
                self._config_dict = json.loads(open(filename()).read())
            except json.JSONDecodeError:
                try:
                    self._config_dict = json.loads(open(temp_file).read())
                    self.save_config()
                except (FileNotFoundError, json.JSONDecodeError):
                    os.remove(filename())
                    self._config_dict = dict()

        else:
            self._config_dict = dict()

        self.__create_backup__()
        self._scheduler.start()
        self._scheduler.enter_recurring(5 * 60, 1, self.__create_backup__)

    def stop(self):
        self._scheduler.stop(True)

    def __create_backup__(self):
        try:
            with open(temp_file, 'w') as f:
                f.write(json.dumps(self._config_dict))

            return True
        except Exception:
            return False

    def __sort_dict__(self):
        temp_dict = dict()
        for key in sorted(self._config_dict.keys()):
            temp_dict[key] = self._config_dict[key]
        self._config_dict = temp_dict

    def save_config(self):
        self.__sort_dict__()
        try:
            with open(filename(), 'w') as f:
                f.write(json.dumps(self._config_dict))
        except IOError:
            pass


# noinspection PyPep8Naming
class config:
    _instance = None

    @staticmethod
    def init():
        config._instance = Config()
        config._instance.initialize()

    @staticmethod
    def stop():
        config._instance.stop()

    @staticmethod
    def get(key, default=None):
        return default if config._instance[key] is None else config._instance[key]

    @staticmethod
    def set(key, value, save=True):
        config._instance[key] = value
        if save:
            config.save_config()

    @staticmethod
    def delete(key):
        try:
            del config._instance[key]
            config.save_config()
        except KeyError:
            pass

    @staticmethod
    def save_config():
        config._instance.save_config()
