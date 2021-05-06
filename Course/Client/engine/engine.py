import time
from threading import Thread

from logger.logger import logger, LogMessageType
from engine.fishing.event import FishEvent
from engine.fishing.mode import FishingMode
from engine.game.client import WindowClient


class FisherEngine:
    def __init__(self, event=None):
        self.event = (lambda x: x) if event is None else event
        self.start = False
        self.window = None
        self.thread = None
        self.client = None

    def run(self):
        FishEvent.init()

        self.client = WindowClient()

        self.event(True)

        if self.event:
            logger.log("Starting the bot engine, look at the fishing hole to start fishing", LogMessageType.Info)
            Thread(target=self.__wait_and_check__).start()

        while self.start and WindowClient.running():
            capture = self.client.get_capture()

            if capture is None:
                self.event(False)
                self.toggle_start()
                continue

            self.client.crop = None
            FishingMode.loop(capture[0][0])

        logger.log("Fishing engine stopped", LogMessageType.Info)
        self.event(False)

        FishEvent.unsubscribe()
        self.client.destroy()

    def __wait_and_check__(self):
        time.sleep(10)
        if not FishEvent.started and self.start:
            logger.log("Doesn't look like fishing has started", LogMessageType.Warning)

    def toggle_start(self):
        self.start = not self.start

        if self.start:
            self.thread = Thread(target=self.run)
            self.thread.start()
