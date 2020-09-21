while True:
    try:
        a = float(input("Введите значение a: "))
        b = float(input("Введите значение b: "))
        c = float(input("Введите значение c: "))

        if a == 0: #Если а = 0 то решаем линейное уравнение
            if b == 0 and c == 0:
                print("Бесконечное количество решений.")
            elif b != 0 and (c == 0 or abs(b) <= abs(c)) and (c % b) == 0:
                print("x = " + str(-c//b))
            else: 
                print("Корней нет.")

            break;

        d = b**2 - 4*a*c
        
        if (d > 0):
            x1 = (-b+d**0.5)/(2*a)
            x2 = (-b-d**0.5)/(2*a)
            print("x1 = " + str(x1))
            print("x2 = " + str(x2))

        elif (d == 0):
            print("x = " + str(-b/(2*a)))
        elif (d < 0):
            print("Действительных корней нет.")

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")