def read():
    while True:
        try:
            hour, minute = map(int, input().split(":"));

            if hour < 0 or hour > 23 or minute < 0 or minute > 59:
                print("Неверное время. Повторите ввод.")
                continue

            return hour * 60 + minute

        except ValueError:
            print("Ошибка парсинга значения. Попробуйте повторить ввод.")


while True:
    try:
        first = read()
        second = read()

        if abs(second - first) <= 15:
            print("Встреча состоится")
        else:
            print("Встреча не состоится")

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
