class Point:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def different(self, second):
        return Point(self.x - second.x, self.y - second.y)


if __name__ == "__main__":
    from start import start
    start()
