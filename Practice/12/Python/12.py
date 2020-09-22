while True:
    try:
        value = int(input("Введите число: "))

        if value < 0:
            print("Число меньше 0. Повторите ввод.")
            continue

        result = 1
        for i in range(2, value + 1):
            result *= i

        print(result)

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
