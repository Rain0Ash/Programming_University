from math import log2

while True:
    try:
        value = int(input("Введите число: "))

        if value <= 0:
            count = 0
        else:
            count = int(log2(value)) + 1  # Используем оптимизированную функцию log2 вместо написания цикла.

        print(count)

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
