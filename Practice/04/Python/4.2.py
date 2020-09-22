while True:
    try:
        a, b = map(int, input("Введите первое и второе число, разделив их пробелом: ").split())

        a, b = b, a

        print("", a, b, sep='\n')
        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")