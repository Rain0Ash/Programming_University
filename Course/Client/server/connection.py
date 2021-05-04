import requests
from requests.exceptions import SSLError

from warnings import filterwarnings
filterwarnings("ignore")


class Connection:
    verify = True
    apikey = None
    login = None
    protocol = None

    def __init__(self, host, port, https=True, verify=True):
        self.host = host
        self.port = port
        self.protocol = "https" if https else "http"
        self.verify = verify is not False

    def authorize(self, login, password, eula=None):
        try:
            if eula is None:
                req = requests.get(f"{self.protocol}://{self.host}:{self.port}/register?login={login}&&password={password}", verify=self.verify)
            elif eula:
                req = requests.get(f"{self.protocol}://{self.host}:{self.port}/register?login={login}&&password={password}&&eula=true", verify=self.verify)
            else:
                return 403

            if req.status_code == 200:
                self.apikey = req.text
                self.login = login
                return self.apikey

            return req.status_code
        except SSLError:
            return 495
        except Exception:
            return 0

    def fishing_transaction(self):
        try:
            req = requests.get(f"{self.protocol}://{self.host}:{self.port}/fishing?key={self.apikey}", verify=self.verify)
            return req.status_code
        except Exception:
            return 0

    def get_user_statistics(self, start=0, end=0x7fffffff, take=10, skip=0):
        if not self.apikey:
            return 0

        start = int(start)
        end = int(end)

        try:
            req = requests.get(f"{self.protocol}://{self.host}:{self.port}/statistics?key={self.apikey}&&start={start}&&end={end}&&take={take}&&skip={skip}", verify=self.verify)
            if req.status_code == 200:
                return req.text

            return req.status_code
        except Exception:
            return 0

    def get_records(self, start=0, end=0x7fffffff, take=10, skip=0):
        start = int(start)
        end = int(end)
        take = int(take)
        skip = int(skip)

        try:
            req = requests.get(f"{self.protocol}://{self.host}:{self.port}/records?start={start}&&end={end}&&take={take}&&skip={skip}", verify=self.verify)

            if req.status_code == 200:
                return req.text

            return req.status_code

        except Exception:
            return 0

    def get_user_records(self, start=0, end=0x7fffffff, take=10, skip=0):
        start = int(start)
        end = int(end)
        take = int(take)
        skip = int(skip)

        try:
            if self.apikey:
                req = requests.get(f"{self.protocol}://{self.host}:{self.port}/records?key={self.apikey}&&start={start}&&end={end}&&take={take}&&skip={skip}", verify=self.verify)
            else:
                return 403

            if req.status_code == 200:
                return req.text

            return req.status_code

        except Exception:
            return 0

    def close(self):
        try:
            if not self.apikey:
                return 200

            req = requests.get(f"{self.protocol}://{self.host}:{self.port}/close?key={self.apikey}", verify=self.verify)
            return req.status_code
        except Exception:
            return 0


class connection:
    _instance = None

    @staticmethod
    def init(host, port, https=True, verify=True):
        if connection._instance is not None:
            connection.close()

        connection._instance = Connection(host, port, https, verify)

    @staticmethod
    def authorize(login, password, eula=None):
        if connection._instance is None:
            raise ConnectionError

        return connection._instance.authorize(login, password, eula)

    @staticmethod
    def fishing_transaction():
        if connection._instance is None:
            raise ConnectionError

        return connection._instance.fishing_transaction()

    @staticmethod
    def get_user_statistics(start=0, end=0x7fffffff, take=10, skip=0):
        if connection._instance is None:
            raise ConnectionError

        return connection._instance.get_user_statistics(start, end, take, skip)

    @staticmethod
    def get_records(start=0, end=0x7fffffff, take=10, skip=0):
        if connection._instance is None:
            raise ConnectionError

        return connection._instance.get_records(start, end, take, skip)

    @staticmethod
    def get_user_records(start=0, end=0x7fffffff, take=10, skip=0):
        if connection._instance is None:
            raise ConnectionError

        return connection._instance.get_user_records(start, end, take, skip)

    @staticmethod
    def close():
        if connection._instance is None:
            return

        return connection._instance.close()
