#include <iostream>
#include <fstream>
#include <string.h>
#include <cassert>
using namespace std;

void write_text(const char *txt, const char *filename){
  FILE *fp = fopen(filename, "w");
  assert(fp != NULL);
  fwrite(txt, 1, strlen(txt), fp);
  fclose(fp);
}

char buf[128];
constexpr int half_n = 44;
int indices[half_n];

int main(){
  ifstream info("test/information.txt");
  int speech_num;
  info >> speech_num;
  cerr << "Run TIme: " << (180.0/17*(speech_num-3) + 60) << "[s]\n";
  sprintf(buf, "constexpr int m = %d;\n", speech_num);
  write_text(buf, "select_num.hpp"); // update speech num
  for(int i = 0; i < speech_num; i++) info >> indices[i];
  write_text("Input\n", "in.txt");
#if defined(_WIN32) || defined(_WIN64)
  system("g++ yakinamashi.cpp -Ofast");
  system("a.exe < in.txt > out.txt");
  system("g++ yakinamashi_thread.cpp -Ofast -fopenmp -lgomp");
  system("copy out.txt in.txt");
  system("a.exe < in.txt > out.txt");
#else
  system("g++ yakinamashi.cpp -Ofast");
  system("./a.out < in.txt > out.txt");
  system("g++ yakinamashi_thread.cpp -Ofast -fopenmp -lgomp");
  system("cp out.txt in.txt");
  system("./a.out < in.txt > out.txt");
#endif
  ifstream result("out.txt");
  string s; result >> s; // "Output"
  // trial
  if(!info.eof()){
    int res[half_n];
    for(int i = 0; i < speech_num; i++) result >> res[i];
    int audio_diff_num = 0;
    int karuta_diff_num = 0;
    for(int i = 0; i < speech_num; i++){
      bool ok = false, ok2 = false;
      for(int j = 0; j < speech_num; j++){
        if(res[i] == indices[j]){
          ok = ok2 = true;
          break;
        }else if(res[i] % half_n == indices[j] % half_n){
          ok2 = true;
        }
      }
      if(!ok) audio_diff_num++;
      if(!ok2) karuta_diff_num++;
    }
    cerr << "Audio Diff: " << audio_diff_num << "/" << speech_num << "\n";
    cerr << "Karuta Diff: " << karuta_diff_num << "/" << speech_num << "\n";
    cout << audio_diff_num << " " << karuta_diff_num << "\n";
  }
  // production
  else{
    for(int i = 0; i < speech_num; i++){
      int a; result >> a;
      cout << a << " ";
    }
    cout << "\n";
  }
}