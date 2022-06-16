#include <stdio.h>
#include "audio/wave.hpp"
//a.exe
//or
//a.exe [directory name]
char buf[64];
int main(int argc, char *args[]){
  Wave wave;
  if(argc == 1){
    read_audio(wave, "./problem.wav");
  }else if(argc == 2){
    sprintf(buf, "./%s/problem.wav", args[1]);
    read_audio(wave, buf);
  }else{
    puts("arguments are not correct");
    return 1;
  }
  change_sampling_hz(wave, analyze_sampling_hz);
  for(const auto &x : wave.data) printf("%d ", x);
  printf("%d\n", (unsigned int)-1 >> 1);
  if(argc == 2){
    sprintf(buf, "./%s/information.txt", args[1]);
    FILE *fp = fopen(buf, "r");
    assert(fp != NULL);
    int m;
    fscanf(fp, "%d", &m);
    for(int i = 0; i < m; i++){
      int t;
      fscanf(fp, "%d", &t);
      printf("%d ", t);
    }
    printf("\n");
  }
}