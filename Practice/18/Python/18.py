words = ("hallo", "klempner", "das", "ist", "fantastisch", "fluggegecheimen")

secret = str(input())

chars = [0] * 26

a = ord('a')

for word in words:
    for c in word:
        chars[ord(c) - a] += 1

chance = 1
total = sum(chars)

for c in secret:
    chance *= chars[ord(c) - a] / total
    if (chance == 0): break

print(chance)
