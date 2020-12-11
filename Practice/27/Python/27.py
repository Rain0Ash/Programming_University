from heapq import *

class wdesc:
    def __init__(self, entity):
        self.entity = entity

    def __lt__(self, o):
        return self.entity.__gt__(o.entity)

    def __repr__(self):
        return self.__str__()
        
    def __str__(self):
        return str(self.entity)


n = input("Введите число n: ")
values = map(int, input("Введите n чисел, разделённые пробелом: ").split(" "))

const = 5

heap = []
heapify(heap)

for value in values:
    value = wdesc(value)
    
    if len(heap) >= const and value > max(heap):
        heappushpop(heap, value)
    elif len(heap) < const:
        heappush(heap, value)
        
    print(' '.join(map(str, sorted(heap))))