from typing import List

import cv2
import imutils

from engine.game.server import WindowServer, Status
from logger.logger import logger, LogMessageType
from helper import helper


class WindowClient:
    clients: List['WindowClient'] = []

    def __init__(self, crop=None, color=None, scale=None, show_name=None):
        self.color = color
        self.crop = crop
        self.scale = scale
        self.show_name = show_name

        if len(WindowClient.clients) == 0:
            WindowServer.start()

        WindowClient.clients.append(self)

    def destroy(self):
        if self in WindowClient.clients:
            WindowClient.clients.remove(self)

        if len(WindowClient.clients) == 0:
            WindowServer.stop()

    @staticmethod
    def running():
        return WindowServer.status == Status.Running

    def get_capture(self):
        if WindowServer.status == Status.Crashed:
            return None

        if not WindowServer.is_screen_ready():
            helper.wait_until(WindowServer.is_screen_ready)

        screen = WindowServer.Screen

        if screen is None or screen.size == 0:
            return None

        if self.color is not None:
            screen = cv2.cvtColor(screen, self.color)

        if self.crop is not None:
            screen = screen[self.crop[1]:self.crop[3], self.crop[0]:self.crop[2]]

        if self.scale is not None:
            screen = cv2.resize(screen, (self.scale[0], self.scale[1]), interpolation=cv2.INTER_AREA)

        return screen

    def processed_image(self, handler=None):
        if WindowServer.status == Status.Crashed:
            return None

        image = self.get_capture()

        if image is None:
            return None

        if handler is None:
            return image

        return handler(image)

    def show(self, show, resize=None, handler=None):
        if WindowServer.status == Status.Crashed:
            return

        if not self.show_name:
            logger.log("You need to assign a name first", LogMessageType.Error)
            return

        if not show:
            cv2.destroyWindow(self.show_name)
            return

        image = self.processed_image(handler)

        if image is None:
            return

        if resize is not None:
            image = imutils.resize(image, width=resize)

        cv2.imshow(self.show_name, image)
        cv2.waitKey(25)
