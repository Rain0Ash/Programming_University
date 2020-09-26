from random import randrange as rand

def read():
    while True:
        try:
            return int(input())
        except ValueError:
            print("Ошибка парсинга значения. Попробуйте повторить ввод.")


while True:
    try:
        print("Угадай число за 5 попыток.")
        result = rand(101)
        count = 5

        while True:
            value = read()

            if value == result:
                print("Поздравляю! Вы угадали")
            elif value > result:
                print("Загаданное число меньше")
                count -= 1
            elif value < result:
                print("Загаданное число больше")
                count -= 1

            if count > 0 and value != result:
                continue
            elif count == 0:
                print("Вы проиграли. Было загадано: {0}".format(result))

            print("Хотите начать сначала? (1 - ДА )")
            if read() == 1:
                break
            else:
                exit(0)

    except KeyboardInterrupt:
        break
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")