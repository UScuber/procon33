import csv
import subprocess
from subprocess import PIPE


file_path = "./results.csv"
train_num = 2

def build():
  subprocess.run("generator.exe test 20 1 8", stderr=PIPE)
  with open("in.txt", "w") as f:
    subprocess.run("tester.exe test", stdout=f)

def run_program():
  build()
  subprocess.run("g++ yakinamashi_thread.cpp -Ofast")
  with open("in.txt", "r") as f:
    process = subprocess.run("a.exe", stdin=f, stdout=PIPE, stderr=PIPE, text=True)
  result = process.stdout
  print(result, end='')
  score, audio, karuta = map(int, result.split())
  return score, audio, karuta

def main():
  with open(file_path, "a", newline='\n') as write_file:
    writer = csv.writer(write_file)
    for i in range(train_num):
      print("start:", i)
      score, audio, karuta = run_program()
      writer.writerow([ score, audio, karuta ])
  

if __name__ == "__main__":
  subprocess.run("g++ generator.cpp -O2 -o generator.exe")
  subprocess.run("g++ tester.cpp -O2 -o tester.exe")
  main()
