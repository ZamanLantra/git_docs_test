
strs = ["eat", "tea", "tan", "ate", "nat", "bat"]

ana = {}

for word in strs:
    lst = tuple(set(word))
    if lst not in ana:
        ana[lst] = []
    ana[lst].append(word)

print(ana.values())