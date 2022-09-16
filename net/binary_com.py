import sys

fr1 = open('output.wav', 'rb')
fr2 = open("problem.wav", "rb")

cnt = 0

while True:
    data1 = fr1.read(1)
    data2 = fr2.read(1)
    if len(data1) == 0 or len(data2) == 0:
        break
    if data1 == data2:
        print("OK")
    else:
        print(cnt)
        print("NG")
        sys.exit()
    cnt += 1

fr1.close()
fr2.close()