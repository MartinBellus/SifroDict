import unidecode
file = open("./sk.txt","r",encoding="utf-8")
out = open("./sk_parsed.txt","w")
for i in file:
    try:
        word, count = i.split(' ')
    except:
        word = i
        if i.isnumeric(): continue
    out.write(unidecode.unidecode(word).lower() + "\n")