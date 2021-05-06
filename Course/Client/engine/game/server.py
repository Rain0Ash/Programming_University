from enum import Enum
from threading import Thread

import cv2
import math

import pywintypes
import win32gui
from win32api import GetSystemMetrics

import numpy as np
from PIL import ImageGrab

from logger.logger import logger, LogMessageType
import helper.config


class Status(Enum):
    Crashed = -1
    Stopped = 0
    Running = 1


class WindowServer:
    Screen = None
    window_offset = None
    title_offset = None
    hwnd = None
    status = Status.Stopped

    @staticmethod
    def init():
        try:
            WindowServer.hwnd = win32gui.FindWindow(None, "Elder Scrolls Online")
            rectangle = win32gui.GetWindowRect(WindowServer.hwnd)
            client_rectangle = win32gui.GetClientRect(WindowServer.hwnd)

            WindowServer.window_offset = math.floor(((rectangle[2] - rectangle[0]) - client_rectangle[2]) / 2)
            WindowServer.title_offset = ((rectangle[3] - rectangle[1]) - client_rectangle[3]) - WindowServer.window_offset

            if helper.config.config.get("borderless"):
                WindowServer.title_offset = 0

            WindowServer.status = Status.Running

        except pywintypes.error:
            logger.log("Game window not found", LogMessageType.Warning)
            WindowServer.status = Status.Crashed

    @staticmethod
    def loop():
        box = (0, 0, GetSystemMetrics(0), GetSystemMetrics(1))

        screen = np.array(ImageGrab.grab(bbox=box))

        rectangle = win32gui.GetWindowRect(WindowServer.hwnd)

        crop = (rectangle[0] + WindowServer.window_offset, rectangle[1] + WindowServer.title_offset,
                rectangle[2] - WindowServer.window_offset, rectangle[3] - WindowServer.window_offset)

        WindowServer.Screen = screen[crop[1]:crop[3], crop[0]:crop[2]]

        if WindowServer.Screen.size == 0:
            logger.log("Don't minimize or drag game window outside the screen", LogMessageType.Error)
            WindowServer.status = Status.Crashed

    @staticmethod
    def loop_end():
        cv2.waitKey(25)

    @staticmethod
    def run():
        while WindowServer.status == Status.Running:
            WindowServer.loop()

        WindowServer.loop_end()

    @staticmethod
    def start():
        if WindowServer.status == Status.Running:
            return

        WindowServer.init()

        if WindowServer.status == Status.Running:
            Thread(target=WindowServer.run).start()

    @staticmethod
    def is_screen_ready():
        return WindowServer.Screen is not None or WindowServer.status == Status.Crashed

    @staticmethod
    def stop():
        WindowServer.status = Status.Stopped
