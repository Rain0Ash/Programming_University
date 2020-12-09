from collections import Counter

def factors(n):
    n = int(n)
    
    while n > 1:
        for i in range(2, int(n) + 1):
            if n % i == 0:
                n /= i
                yield i
                break


if __name__ == "__main__":
    print('*'.join([(str(factor) + "^" + str(count)) if count > 1 else str(factor) for factor, count in sorted(dict(Counter(factors(int(input())))).items())]))