#include <algorithm>
#include <string>
#include <string.h>
#include "../audio/wave.hpp"
using std::vector;

FILE *fp;

constexpr int n = 44;
constexpr int sampling_hz = 12000;
constexpr int inf = (unsigned int)-1 >> 1;
char buf[64];
Wave waves[n][2];
int length = 0;
int main(){
  fp = fopen("./audio_array.hpp", "w");
  assert(fp != NULL);
  for(int i = 0; i < n; i++){
    sprintf(buf, "../audio/JKspeech/J%02d.wav", i+1);
    read_audio(waves[i][0], buf);
    change_sampling_hz(waves[i][0], sampling_hz);
    sprintf(buf, "../audio/JKspeech/E%02d.wav", i+1);
    read_audio(waves[i][1], buf);
    change_sampling_hz(waves[i][1], sampling_hz);
    length = std::max({ length, waves[i][0].L, waves[i][1].L });
  }
  printf("max length: %d\n", length);
  sprintf(buf, "constexpr int arrays[n*2][%d] = {\n", length);
  fwrite(buf, 1, strlen(buf), fp);
  for(int lang = 0; lang < 2; lang++){
    for(int i = 0; i < n; i++){
      std::string s = "  { ";
      for(int j = 0; j < waves[i][lang].L; j++) s += std::to_string(waves[i][lang][j]) + ",";
      if(waves[i][lang].L < length) s += std::to_string(inf) + ",";
      s += " },\n";
      fwrite(s.c_str(), 1, s.size(), fp);
    }
  }
  fwrite("};\n", 1, 3, fp);
  fclose(fp);
}