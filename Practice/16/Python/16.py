import re


def match(ticket):
    return re.match("^a(?:(?:[a-z][0-9]{2})|(?:[0-9][a-z][0-9])|(?:[0-9]{2}[a-z]))55661$", ticket)


def read_and_validate_tickets():
    return [x for x in map(str, input().split(" ")) if match(x)]


while True:
    try:
        n = int(input("Введите число: "))

        if n < 1:
            print("Число меньше 1. Повторите ввод.")
            continue

        tickets = read_and_validate_tickets()

        print(' '.join(tickets) if tickets else -1)

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
