def bmi(weight: float, height: float) -> float:
    return weight / height ** 2


def print_bmi(bmi: float) -> float:
    if bmi < 18.5:
        print("Underweight")
    elif bmi < 25:
        print("Normal")
    elif bmi < 30:
        print("Overweight")
    else:
        print("Obesity")


while True:
    try:
        weight, height = map(float, input().split())

        if weight <= 0 or height <= 0:
            print("Одно или несколько чисел меньше нуля.")
            continue

        print_bmi(bmi(weight, height / 100))

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Повторите ввод.")
