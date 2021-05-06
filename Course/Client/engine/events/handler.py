import time
from threading import Thread

from engine.engine import FisherEngine


class EngineEventHandler:
    def __init__(self, event=None):
        self.event_handler_running = True
        self.event = []

        self.fisher_engine = FisherEngine(event)

    def is_started(self):
        return self.fisher_engine is not None and self.fisher_engine.start

    def start_event_handler(self):
        while self.event_handler_running:
            while len(self.event) > 0:
                event = self.event.pop(0)
                event()

            time.sleep(0.1)

    def toggle_fisher(self):
        self.event.append(self.fisher_engine.toggle_start)

    def quit(self):
        def stop():
            self.fisher_engine.start = False
            self.event_handler_running = False

        self.event.append(stop)


class engine_event_handler:
    __bot__ = None

    @staticmethod
    def init(event=None):
        engine_event_handler.__bot__ = EngineEventHandler(event)

    @staticmethod
    def is_started(start=None):
        if start is None:
            return engine_event_handler.__bot__ is not None and engine_event_handler.__bot__.is_started()

        if start:
            if engine_event_handler.__bot__ is None:
                engine_event_handler.init()
                bot = Thread(target=engine_event_handler.start_event_handler)
                bot.start()

            engine_event_handler.toggle_fisher()
            return start
        else:
            if engine_event_handler.is_started():
                engine_event_handler.toggle_fisher()

            return start

    @staticmethod
    def start_event_handler():
        if engine_event_handler.__bot__ is None:
            raise TypeError

        engine_event_handler.__bot__.start_event_handler()

    @staticmethod
    def toggle_fisher():
        if engine_event_handler.__bot__ is None:
            raise TypeError

        engine_event_handler.__bot__.toggle_fisher()

    @staticmethod
    def quit():
        if engine_event_handler.__bot__ is None:
            return

        engine_event_handler.__bot__.quit()
