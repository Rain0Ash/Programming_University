words = ("hallo", "klempner", "das", "ist", "fantastisch", "fluggegecheimen")

word = str(input())

chars = [0] * 26

a = ord('a')

for wrd in words:
    for c in wrd:
        chars[ord(c) - a] += 1

chance = 1
total = sum(chars)

for c in word:
    chance *= chars[ord(c) - a] / total
    if (chance == 0): break

print(chance)
