from collections import Counter

def factors(value):
    value = int(value)
    
    while value > 1:
        for i in range(2, value + 1):
            if value % i == 0:
                value //= i
                yield i
                break


if __name__ == "__main__":
    n = int(input())
    print('*'.join([(str(factor) + "^" + str(count)) if count > 1 else str(factor) for factor, count in sorted(dict(Counter(factors(n))).items())]) if n > 3 else n)