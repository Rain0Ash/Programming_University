init = False


def __init__():
    global init

    if init:
        return

    from os.path import dirname, abspath
    from sys import path
    from inspect import getfile, currentframe

    currentdir = dirname(abspath(getfile(currentframe())))
    parentdir = dirname(currentdir)
    path.insert(0, parentdir)

    init = True


__init__()


def start():
    from view import View
    from model import Model

    size = 10
    view = View(size)
    model = Model(view)

    view.start()


if __name__ == "__main__":
    start()
