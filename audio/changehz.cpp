#include <iostream>
#include <string>
#include "wave.hpp"

//a.exe [import file name] [export file name] [sampling hz]
int main(int argc, char *args[]){
  Wave prm;
  read_audio(prm, args[1]);
  printf("bits:%d, sampling hz:%d\n", prm.bits, prm.fs);
  change_sampling_hz(prm, std::stoi(args[3]));
  write_audio(prm, args[2]);
}
