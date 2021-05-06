class LogMessageType:
    Info = 0
    Successful = 1
    Warning = 2
    Error = 3
    Fatal = 4


class Logger:
    def __init__(self):
        self.__events__ = []

    def subscribe(self, event):
        if event is not None:
            self.__events__.append(event)

    def unsubscribe(self, event):
        if event is not None:
            try:
                self.__events__.remove(event)
            except ValueError:
                pass

    def log(self, message, msg_type=LogMessageType.Info):
        for event in self.__events__:
            try:
                event(message, msg_type)
            except Exception:
                pass


class logger:
    __logger__ = None

    @staticmethod
    def init():
        if logger.__logger__ is not None:
            return

        logger.__logger__ = Logger()
        from interface.app import js_log_notify
        logger.__logger__.subscribe(js_log_notify)

    @staticmethod
    def subscribe(event):
        if logger.__logger__ is None:
            raise TypeError

        logger.__logger__.subscribe(event)

    @staticmethod
    def unsubscribe(event):
        if logger.__logger__ is None:
            raise TypeError

        logger.__logger__.unsubscribe(event)

    @staticmethod
    def log(message, msg_type=LogMessageType.Info):
        if logger.__logger__ is None:
            raise TypeError

        logger.__logger__.log(message, msg_type)
