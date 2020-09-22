while True:
    try:
        first, operation, second = input("Введите число1 (+-*/) число2\n").split()

        first = float(first)
        second = float(second)

        if operation == '+':
            value = first + second
        elif operation == '-':
            value = first - second
        elif operation == '*':
            value = first * second
        elif operation == '/':
            value = first / second
        else:
            print("Неизвестный символ операции. Попробуйте повторить ввод.")
            continue

        if value % 1 == 0:
            value = int(value)

        print(value)

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
