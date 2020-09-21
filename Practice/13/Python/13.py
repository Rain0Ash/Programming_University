while True:
    try:
        value = int(input())

        if value < 2: print("Число < 2")
        elif value == 2: print("Простое") #Проводим некоторые оптимизации, в частности с числом 2
        elif value % 2 == 0: print("Составное")
        else:
            divisor = 3 #Проводим поиск только в нечетных делителях.
            while divisor ** 2 <= value and value % divisor != 0: #Также наименьший делитель не может быть больше квадратного корня из определяемого числа
                divisor += 2
            
            if divisor ** 2 <= value: print("Составное")
            else: print("Простое")

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")