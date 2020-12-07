from random import randint
import math
# Python не позволяет проводить перегрузку функций, поэтому будет использоваться
# декоратор dispatch из устанавливаемого пакета multipledispatch, который
# позволяет ее выполнить обходными путями
from multipledispatch import dispatch


def more(first, second): return first > second


def less(first, second): return first < second


def is_sorted(arr, asc=True):
    sort = more if asc else less

    for i in range(1, len(arr)):
        if sort(arr[i - 1], arr[i]):
            return False

    return True


@dispatch(list, bool)
def BozoSort(arr, asc=True):
    result = []

    if len(arr) <= 1:
        return result

    # также в python невозможно определить что в фунцию передан список из списков не проводя проверку
    # во время исполнения, поэтому даже с декоратором dispatch нельзя определить перегрузку как list->list
    if type(arr[0]) == list:
        for lst in arr:
            result += lst
    else:
        result = arr.copy()

    size = len(result)
    while not is_sorted(result, asc):
        first = randint(0, size - 1)
        second = randint(0, size - 1)

        result[first], result[second] = result[second], result[first]

    return result


@dispatch(int, int, int, bool)
def BozoSort(a, b, c, asc=True):
    return BozoSort([a, b, c], asc)


if __name__ == "__main__":
    sqrt = int(input("Введите квадрат целого числа: ")) ** 0.5

    if sqrt % 1 != 0:
        print("Ошибка, число не является квадратом")

    sqrt = int(sqrt)

    print("Введите значения через пробел: ", end="")
    lst = list(map(int, input().split(' ')))
    matrix = []
    row = []
    i = 0
    for item in lst:
        row.append(item)
        i += 1
        if i % sqrt == 0:
            matrix.append(row.copy())
            row.clear()
            i = 0

    print(' '.join(map(str, BozoSort(lst, True))))
    print(' '.join(map(str, BozoSort(lst, False))))
    print(' '.join(map(str, BozoSort(matrix, True))))
    print(' '.join(map(str, BozoSort(matrix, False))))
    print(' '.join(map(str, BozoSort(*lst[:3], True))))
    print(' '.join(map(str, BozoSort(*lst[:3], False))))
