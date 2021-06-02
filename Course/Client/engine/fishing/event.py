import math
import random
import time
from threading import Thread

import keyboard
from win32gui import GetWindowText, GetForegroundWindow

from engine.fishing.mode import State, FishingMode
from logger.logger import logger, LogMessageType
from helper.config import config
from server import connection


class FishEvent:
    started = False
    jitter = True
    state = State["WAITING"]

    action_key = 'e'
    collect_key = 'r'

    @staticmethod
    def reaction_time(lower_limit_ms, upper_limit_ms):
        reaction = math.fabs(random.gauss(0.225, 0.6) * 1000)

        if reaction < lower_limit_ms:
            return lower_limit_ms

        if reaction > upper_limit_ms:
            return upper_limit_ms

        return reaction

    @staticmethod
    def __fishing_sleep__(wait_time_ms=None, lower_limit_ms=200, upper_limit_ms=750):
        reaction = 500
        if FishEvent.jitter and upper_limit_ms > lower_limit_ms:
            reaction = FishEvent.reaction_time(lower_limit_ms, upper_limit_ms)

        if not wait_time_ms:
            wait_time_ms = reaction

        wait_time_ms = min(wait_time_ms, upper_limit_ms)
        time.sleep(wait_time_ms / 1000)

    @staticmethod
    def init():
        FishEvent.subscribe()
        FishEvent.jitter = config.get("jitter", True)
        FishEvent.action_key = config.get("action_key", 'e')
        FishEvent.collect_key = config.get("collect_key", 'r')

    @staticmethod
    def unsubscribe():
        if FishEvent.fisher_callback in FishingMode.__subscribers__:
            FishingMode.__subscribers__.remove(FishEvent.fisher_callback)

    @staticmethod
    def subscribe():
        if FishEvent.fisher_callback not in FishingMode.__subscribers__:
            FishingMode.__subscribers__.append(FishEvent.fisher_callback)

            if FishingMode.CurrentMode == ["LOOKING"]:
                FishEvent.fisher_callback(FishingMode.CurrentMode)

    @staticmethod
    def fisher_callback(event):
        callbacks_map = {
            "WAITING": FishEvent.on_idle,
            "LOOKAWAY": FishEvent.on_lookaway,
            "LOOKING": FishEvent.on_looking,
            "DEPLETED": FishEvent.on_depleted,
            "NOBAIT": FishEvent.on_nobait,
            "FISHING": FishEvent.on_fishing,
            "REELIN": FishEvent.on_reelin,
            "LOOT": FishEvent.on_loot,
            "INVFULL": FishEvent.on_invfull,
            "FIGHT": FishEvent.on_fight,
            "DEAD": FishEvent.on_dead
        }

        try:
            callbacks_map[event]()
            FishEvent.state = event
        except KeyError:
            pass

    @staticmethod
    def on_idle():
        if FishEvent.state in (State["FISHING"], State["REELIN"]):
            logger.log("FISHING INTERRUPTED", LogMessageType.Info)

    @staticmethod
    def on_lookaway():
        return

    @staticmethod
    def on_looking():
        if GetWindowText(GetForegroundWindow()) != "Elder Scrolls Online":
            logger.log("ESO window is not focused", LogMessageType.Warning)
            return

        FishEvent.__fishing_sleep__()
        keyboard.press_and_release(FishEvent.action_key)

    @staticmethod
    def on_depleted():
        logger.log("Hole depleted", LogMessageType.Warning)

    @staticmethod
    def on_nobait():
        logger.log("No bait equipped!", LogMessageType.Warning)

    @staticmethod
    def on_fishing():
        FishEvent.started = True

    @staticmethod
    def on_reelin():
        if GetWindowText(GetForegroundWindow()) != "Elder Scrolls Online":
            logger.log("ESO window is not focused", LogMessageType.Warning)
            return

        Thread(target=connection.fishing_transaction).start()

        FishEvent.__fishing_sleep__()
        keyboard.press_and_release(FishEvent.action_key)
        FishEvent.__fishing_sleep__(500)

    @staticmethod
    def on_loot():
        FishEvent.__fishing_sleep__()
        keyboard.press_and_release(FishEvent.collect_key)
        FishEvent.__fishing_sleep__()

    @staticmethod
    def on_invfull():
        logger.log("Inventory full!", LogMessageType.Warning)

    @staticmethod
    def on_fight():
        logger.log("Fighting!", LogMessageType.Warning)

    @staticmethod
    def on_dead():
        logger.log("Character is dead!", LogMessageType.Warning)
