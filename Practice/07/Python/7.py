def read():
    while True:
        try:
            return float(input())
        except ValueError:
            print("Ошибка парсинга значения. Попробуйте повторить ввод.")


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def distance(self, point):
        return ((self.x - point.x) ** 2 + (self.y - point.y) ** 2) ** 0.5


def read_point():
    while True:
        try:
            return Point(*map(float, input().split()))
        except ValueError:
            print("Ошибка парсинга значения. Попробуйте повторить ввод.")


while True:
    try:
        method = int(input("Введите метод решения: \n1) через длины сторон a,b,c\n2) через координаты вершин A,B,C\n"))

        if method == 1:
            a = read()
            b = read()
            c = read()
        elif method == 2:
            A = read_point()
            B = read_point()
            C = read_point()

            a = A.distance(B)
            b = B.distance(C)
            c = C.distance(A)
        else:
            print("Неизвестный метод решения. Попробуйте повторить ввод.")
            continue

        if a + b <= c or a + c <= b or b + c <= a:
            print("Треугольник не существует. Повторите ввод.", end="\n\n")
            continue

        p = (a + b + c) / 2
        S = (p * (p - a) * (p - b) * (p - c)) ** 0.5

        if S % 1 == 0:
            S = int(S)

        print("S = " + str(S))

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except ZeroDivisionError:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
