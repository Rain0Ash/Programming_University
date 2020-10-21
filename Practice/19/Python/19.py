def convert(value, n, k) -> str:
    result = ''
    while value > 0:
        result += str(value % k)
        value //= k

    return result[::-1].zfill(n)


while True:
    try:
        n = int(input())
        k = tuple(input())

        if n < 1 or n > 8:
            print("Число n находится вне границ [1; 8]. Повторите ввод.")
            continue

        if len(k) < 1 or len(k) > n:  # Функция len имеет сложность O(1) для обычных вызовов и не выполняет расчеты.
            print("Длина строки меньше 1 или больше n. Повторите ввод.")
            continue

        result = [convert(x, n, len(k)) for x in range(len(k) ** n)]  # Конвертируем системы счисления

        symbols = set([str(x) for x in range(len(k))])
        result = [x for x in result if set(x) == symbols]  # Проверяем на совпадение чисел

        result = ' '.join([''.join(k[int(j)] for j in r) for r in result])  # Объединяем

        print(result)

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Повторите ввод.")
