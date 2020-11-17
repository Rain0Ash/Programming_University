import json
import os

abspath = os.path.abspath(__file__)
dirname = os.path.dirname(abspath)
os.chdir(dirname)


class Work:
    def __init__(self, user_id, id, title, completed):
        self.userId = user_id
        self.Id = id
        self.Title = title
        self.Completed = completed


def as_work(dct):
    return Work(dct.get("userId"), dct.get("id"), dct.get("title"), dct.get("completed"))


class DataEncoder(json.JSONEncoder):
    def default(self, obj):
        return obj.__dict__


class Data:
    def __init__(self, user_id, count=1):
        self.task_completed = count
        self.userId = user_id


with open("in.json", "r") as read_file:
    works = json.load(read_file, object_hook=as_work)

    with open("out.json", "w") as write_file:
        user_tasks = dict()
        for work in works:
            if not work.Completed:
                continue

            if work.userId in user_tasks:
                user_tasks[work.userId] += 1
            else:
                user_tasks[work.userId] = 1

        json.dump([Data(key, value) for key, value in sorted(user_tasks.items())], write_file, indent=4, cls=DataEncoder)
