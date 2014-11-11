import fileinput
n = 9
for i, line in enumerate(fileinput.input()):
    with open(str(i)+'.txt', "w") as file:
        rows = [line[j:j+n] for j in range(0, len(line), n)]
        for row in rows:
            row = ' '.join(map(lambda c: c == '.' and '0' or c, row))
            print(row, file=file)
