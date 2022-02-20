import unidecode
file = open("./sk.txt","r",encoding="utf-8")
out = open("./sk_parsed.txt","w")
prev = []
for i in file:
    a = list(map(str.strip,i.split(' ')))
    if prev != '':
        a = prev + a
        prev = ''
    a = list(filter(('').__ne__, a))
    #print(a,len(a),a[1].isnumeric())
    if len(a) == 2 and a[1].isnumeric():
        word,f = a
        out.write(unidecode.unidecode(word+" "+f).lower() + "\n")
    elif not a[-1].isnumeric():
        prev = a