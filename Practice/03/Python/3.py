while True:
    try:
        first, second = map(int,input("Введите первое и второе число, разделив их пробелом: ").split())

        print("", "x + y = " + str(first + second), "x - y = " + str(first - second), "y - x = " + str(second - first), "x * y = " + str(first * second), sep='\n')

        if second != 0:
            print("x / y = " + str(first / second))
        else:
            print("x / y = ∞ (Значение y = 0)")

        if first != 0:
            print("y / x = " + str(second / first))
        else:
            print("y / x = ∞ (Значение x = 0)")

        #Аналогом операнда / для двух значений типа int в c++, в языке python является операнд // (целочисленное деление).
        break
    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except ZeroDivisionError:
        print("Ошибка деления на ноль. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")