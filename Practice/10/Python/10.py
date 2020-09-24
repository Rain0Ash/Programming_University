while True:
    try:
        s, l1, r1, l2, r2 = map(int, input().split())

        if l1 + r2 >= s:
            r2 = max(s-l1, l2)
        else:
            l1 = min(s-r2, r1)

        print("{0} {1}".format(l1, r2) if (l1 + r2) == s else -1)

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Попробуйте повторить ввод.")
    except Exception:
        print("Неизвестная ошибка. Попробуйте повторить ввод.")
