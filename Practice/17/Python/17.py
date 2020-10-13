from collections import deque


def read_value():
    while True:
        try:
            current = int(input("Введите число: "))

            if current > 36:
                print("Ошибка. Число больше 36. Повторите ввод")
                continue

            return current

        except ValueError:
            print("Ошибка. Повторите ввод.")


k = 12

redblack = (None,
            True, False, True,
            False, True, False,
            True, False, True,
            False, False, True,
            False, True, False,
            True, False, True,
            True, False, True,
            False, True, False,
            True, False, True,
            False, False, True,
            False, True, False,
            True, False, True)

values = [0] * len(redblack)

queue = deque(maxlen=k)
red = 0
black = 0

while True:
    try:
        value = read_value()

        if value < 0:
            break

        values[value] += 1
        queue.append(value)

        maximum = max(values)
        print(' '.join([str(i) for i, item in enumerate(values) if item == maximum]))

        print(' '.join([str(x) for x in range(37) if x not in queue]))

        color = redblack[value]
        if color is None:
            pass
        elif color:
            red += 1
        elif not color:
            black += 1

        print(red, black)

        print()

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except ZeroDivisionError:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
