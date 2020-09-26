def read():
    while True:
        try:
            return map(int, input("Введите первое и второе число, разделив их пробелом: ").split())
        except ValueError:
            print("Ошибка парсинга значения. Попробуйте повторить ввод.")

while True:
    try:
        print("Первый метод")
        a, b = read()

        # Тут можно провести оптимизацию сравнив a и b и не обменивая их значениями в случае эквивалентности их
        # значений.

        temp = a;
        a = b;
        b = temp;

        print("", a, b, sep='\n')

        print("Второй метод")
        a, b = read()

        a, b = b, a

        print("", a, b, sep='\n')

        break

    except KeyboardInterrupt:
        break
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")