while True:
    try:
        x = int(input())
        y = int(input())

        print()

        result = 0

        if y == 0 or x == 1:
            result = 1
        elif y == 1:
            result = x
        elif x == 0:
            result = 0
        else:
            negative = y < 0
            if negative:
                y = -y

            result = x
            for i in range(y - 1):
                result *= x

            if negative:
                result = 1 / result

        print(result)

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")