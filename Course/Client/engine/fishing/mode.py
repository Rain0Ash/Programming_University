import time

State = {
    "WAITING": [255, 255, 255],
    "LOOKAWAY": [76, 0, 76],
    "LOOKING": [101, 69, 0],
    "DEPLETED": [0, 76, 76],
    "NOBAIT": [255, 204, 0],
    "FISHING": [75, 156, 213],
    "REELIN": [0, 204, 0],
    "LOOT": [0, 0, 204],
    "INVFULL": [0, 0, 51],
    "FIGHT": [204, 0, 0],
    "DEAD": [51, 51, 51]
}


class FishingMode:
    __subscribers__ = []

    CurrentMode = State["WAITING"]
    PrevMode = State["WAITING"]

    @staticmethod
    def __notify__(event):
        for subscriber in FishingMode.__subscribers__:
            subscriber(event)

    @staticmethod
    def loop(rgb):
        FishingMode.CurrentMode = "WAITING"
        for state in State:
            if all(rgb == State[state]):
                FishingMode.CurrentMode = state
                break

        if State[FishingMode.CurrentMode] == State["LOOKING"]:
            FishingMode.__notify__(FishingMode.CurrentMode)
            time.sleep(1)
        elif FishingMode.CurrentMode != FishingMode.PrevMode:
            FishingMode.__notify__(FishingMode.CurrentMode)

        FishingMode.PrevMode = FishingMode.CurrentMode
