import unidecode
file = open("./sk.txt","r",encoding="utf-8")
out = open("./sk_parsed.txt","w")
prev = []
num = 0
for i in file:
    a = list(map(str.strip,i.split(' ')))
    if prev != '':
        a = prev + a
        prev = ''
    a = list(filter(('').__ne__, a))
    if len(a) == 2 and a[1].isnumeric():
        word,f = a
        if len(word) < 36:
            out.write(unidecode.unidecode(word+" "+f).lower() + "\n")
            num+=len(word)
    elif not a[-1].isnumeric():
        prev = a
print(num)