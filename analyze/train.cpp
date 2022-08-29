#include <iostream>
#include <string>
#include <fstream>

const char *file_path = "results2.csv";

void init(){
#if defined(_WIN32) || defined(_WIN64)
  system("g++ generator.cpp -O2 -o generator.exe");
  system("g++ tester.cpp -O2 -o tester.exe");
  system("g++ yakinamashi_thread.cpp -Ofast -fopenmp -lgomp -o yakinamashi.exe");
#else
  system("g++ generator.cpp -O2 -o generator");
  system("g++ tester.cpp -O2 -o tester");
  system("g++ yakinamashi_thread.cpp -Ofast -fopenmp -lgomp -o yakinamashi");
#endif
}

void run_program(){
#if defined(_WIN32) || defined(_WIN64)
  system("generator.exe test 20 1 8");
  system("tester.exe test > in.txt");
  system("yakinamashi.exe < in.txt > out.txt");
#else
  system("./generator test 20 1 8");
  system("./tester test > in.txt");
  system("./yakinamashi < in.txt > out.txt");
#endif
}

int main(int argc, char *argv[]){
  if(argc != 2){
    printf("train.exe [train num]");
    return 0;
  }
  const int train_num = std::stoi(argv[1]);
  init();
  for(int i = 0; i < train_num; i++){
    printf("start: %d\n", i+1);
    run_program();
    std::ofstream result(file_path, std::ios::app);
    std::ifstream output("out.txt");
    int audio, karuta, score;
    output >> audio >> karuta >> score;
    result << audio << "," << karuta << "," << score << ",\n";
    printf("%d, %d, %d\n", audio, karuta, score);
    output.close();
    result.close();
  }
}