#include <iostream>
#include <string>
#include <fstream>

void init(){
#if defined(_WIN32) || defined(_WIN64)
  system("g++ generator.cpp -O2 -o generator.exe");
  system("g++ run.cpp -O2 -o run.exe");
#else
  system("g++ generator.cpp -O2 -o generator");
  system("g++ run.cpp -O2 -o run");
#endif
}

void run_program(){
#if defined(_WIN32) || defined(_WIN64)
  system("generator.exe test 20 1 8");
  system("run.exe > res.txt");
#else
  system("./generator test 20 1 8");
  system("./run > res.txt");
#endif
}

int main(int argc, char *argv[]){
  if(argc != 3){
    printf("error: train.exe [file path] [train num]");
    return 0;
  }
  const int train_num = std::stoi(argv[2]);
  init();
  for(int i = 0; i < train_num; i++){
    printf("START: %d\n", i+1);
    run_program();
    std::ofstream result(argv[1], std::ios::app);
    std::ifstream output("res.txt");
    int audio_diff, karuta_diff, score;
    output >> audio_diff >> karuta_diff >> score;
    result << audio_diff << "," << karuta_diff << "," << score << "\n";
    output.close();
    result.close();
  }
}