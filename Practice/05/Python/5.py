a = 9.8

while True:
    try:
        x0, v0, t = map(float, input("Введите значения x0, v0, t разделив их через пробел: ").split())

        xt = x0 + v0 * t - 1 / 2 * a * t * t
        print("", "Расстояние = " + str(abs(x0 - xt)), sep='\n')
        # Используем функцию abs (модуль числа), так как нам надо получить пройденное расстояние.
        break

        # В отличии от c++ в python 3 деление int/int = float, поэтому 1/2 = 0.5, а не 0, соответственно формулы
        # являются идентичными.
    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
