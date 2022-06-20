#include <stdio.h>
#include "audio/wave.hpp"
//a.exe
//or
//a.exe [directory name]
constexpr int inf = (unsigned int)-1 >> 1;
char buf[64];
int arrays[analyze_audio_max_length][88];
void read_audio_arrays(){
  Wave wave_data;
  for(int i = 0; i < 44; i++){
    sprintf(buf, "audio/JKspeech/J%02d.wav", i+1);
    read_audio(wave_data, buf);
    change_sampling_hz(wave_data, analyze_sampling_hz);
    assert(wave_data.L <= analyze_audio_max_length);
    for(int j = 0; j < wave_data.L; j++) arrays[i][j] = wave_data[j];
    if(wave_data.L < analyze_audio_max_length) arrays[i][wave_data.L] = (unsigned int)-1 >> 1;
  }
  for(int i = 44; i < 88; i++){
    sprintf(buf, "audio/JKspeech/E%02d.wav", i-44+1);
    read_audio(wave_data, buf);
    change_sampling_hz(wave_data, analyze_sampling_hz);
    assert(wave_data.L <= analyze_audio_max_length);
    for(int j = 0; j < wave_data.L; j++) arrays[i][j] = wave_data[j];
    if(wave_data.L < analyze_audio_max_length) arrays[i][wave_data.L] = (unsigned int)-1 >> 1;
  }
}
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

  read_audio_arrays();
  for(int i = 0; i < 88; i++){
    for(int j = 0; j < analyze_audio_max_length; j++){
      printf("%d ", arrays[i][j]);
      if(arrays[i][j] == inf) break;
    }
    printf("\n");
  }

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