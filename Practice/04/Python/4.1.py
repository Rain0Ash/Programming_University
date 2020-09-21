while True:
    try:
        a, b = map(int,input("Введите первое и второе число, разделив их пробелом: ").split())

        #Тут можно провести оптимизацию сравнив a и b и не обменивая их значениями в случае эквивалентности их значений.

        temp = a;
        a = b;
        b = temp;

        print("", a, b, sep='\n')
        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")