#include "../analyze/audio/wave.hpp"

int main(){
  Wave wave,wave2;
  read_audio(wave, "audio/JKspeech/J01.wav");
  read_audio(wave2, "audio/JKspeech/J02.wav");
  Wave res = merge_audio({ wave, wave2 }, { 0, 0 });
  write_audio(res, "twice.wav");
}