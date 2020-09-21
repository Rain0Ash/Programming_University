from random import randrange as rand

def read():
    while True:
        try:
            return int(input())
        except ValueError:
            print("Ошибка парсинга значения. Попробуйте повторить ввод.")

print("Угадай число за 5 попыток.")
result = rand(101)
count = 5

while True:
    try:
        value = read()

        if (value == result):
            print("Поздравляю! Вы угадали")
            break
        elif (value > result): print("Загаданное число меньше")
        elif (value < result): print("Загаданное число больше")

        count -= 1

        if (count > 0): continue

        print("Вы проиграли. Было загадано: {0}".format(result))

        print("Хотите начать сначала? (1 - ДА )")
        value = read()

        if (value == 1):
            print("Угадай число за 5 попыток.")
            result = rand(101)
            count = 5
            continue;

        break

    except KeyboardInterrupt:
        break
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")