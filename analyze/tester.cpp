#include <stdio.h>
#include "audio/wave.hpp"
#define rep(i, n) for(int i = 0; i < (n); i++)

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
  if(argc == 2){
    sprintf(buf, "./%s/information.txt", args[1]);
  }else if(argc == 1){
    sprintf(buf, "./information.txt");
  }
  FILE *fp = fopen(buf, "r");
  assert(fp != NULL);
  int m;
  fscanf(fp, "%d", &m);
  std::vector<int> idx(m), pos(m), len(m), st(m);
  int t,p,l,s;
  rep(i, m){
    fscanf(fp, "%d", &t);
    idx[i] = t;
  }
  rep(i, m){
    fscanf(fp, "%d", &p);
    pos[i] = p;
  }
  rep(i, m){
    fscanf(fp, "%d", &l);
    len[i] = l;
  }
  rep(i, m){
    fscanf(fp, "%d", &s);
    st[i] = s;
  }

  change_sampling_hz(wave, analyze_sampling_hz);
  for(const auto &x : wave.data) printf("%d ", x);
  printf("%d\n", (unsigned int)-1 >> 1);

  rep(i, m) printf("%d%c", idx[i], " \n"[i == m - 1]);
  rep(i, m) printf("%d%c", pos[i], " \n"[i == m - 1]);
  rep(i, m) printf("%d%c", len[i], " \n"[i == m - 1]);
  rep(i, m) printf("%d%c", st[i], " \n"[i == m - 1]);
}