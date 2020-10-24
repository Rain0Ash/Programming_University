from operator import attrgetter

class Alcohol:
    def __init__(self, name, price, volume):
        self.name = name
        self.price = int(price) if int(price) else 1
        self.volume = int(volume)
        self.efficiency = self.volume / self.price


class Money:
    def __init__(self, value: float):
        self.value = value


def read_alcohol() -> Alcohol:
    while True:
        try:
            name, price, volume = input().split()
            return Alcohol(name, int(price), int(volume))

        except ValueError:
            print("Ошибка парсинга значений. Повторите ввод.")


def get_alco_from_market(money: Money, market: tuple) -> dict:
    store = dict()

    market = [x for x in market]  # Дублируем коллекцию

    current = max(market, key=attrgetter('efficiency'))

    if current is None:
        return store

    count = money.value // current.price
    money.value %= current.price
    store[current] = count

    for alco in market:
        if alco.price <= money.value:
            market.remove(current)
            store = {**store, **get_alco_from_market(money, market)}

    return store


def float_for_print(value: float):
    return value if value % 1 else int(value)


while True:
    try:
        money = Money(float(input()))
        n = int(input())

        market = (alco for alco in (read_alcohol() for _ in range(n)) if alco.price <= money.value)

        if not market:
            print(-1)
            break

        total = 0

        for alco, count in get_alco_from_market(money, market).items():
            if count <= 0:
                continue

            total += alco.volume * count
            print(alco.name, float_for_print(count))

        print(float_for_print(total))
        print(float_for_print(money.value))

        break

    except KeyboardInterrupt:
        break
    except ValueError:
        print("Ошибка парсинга значения. Повторите ввод.")
