"""
config.py
Saves configuration in file as json file
"""
import json
import os
from typing import Optional

from .helper import get_main_directory, sort_dict


class Config:
    path = os.path.join(get_main_directory(), "config.json")

    def __init__(self):
        self.__config__: Optional[dict] = None

    def __getitem__(self, item):
        return self.__config__.get(item)

    def __setitem__(self, key, value):
        self.__config__[key] = value

    def __delitem__(self, key):
        del self.__config__[key]

    def initialize(self):
        if not os.path.exists(Config.path):
            self.__config__ = dict()
            return

        try:
            self.__config__ = json.loads(open(Config.path).read())
        except json.JSONDecodeError:
            try:
                self.save_config()
            except (FileNotFoundError, json.JSONDecodeError):
                os.remove(Config.path)
                self.__config__ = dict()

    def save_config(self):
        self.__config__ = sort_dict(self.__config__)

        try:
            with open(Config.path, 'w') as file:
                file.write(json.dumps(self.__config__))
        except IOError:
            pass


# noinspection PyPep8Naming
class config:
    __instance__ = None

    @staticmethod
    def init():
        config.__instance__ = Config()
        config.__instance__.initialize()

    @staticmethod
    def stop():
        config.__instance__.stop()

    @staticmethod
    def get(key, default=None):
        value = config.__instance__[key]
        return default if value is None else value

    @staticmethod
    def set(key, value, save=True):
        config.__instance__[key] = value
        if save:
            config.save_config()

    @staticmethod
    def delete(key):
        try:
            del config.__instance__[key]
            config.save_config()
        except KeyError:
            pass

    @staticmethod
    def save_config():
        config.__instance__.save_config()
