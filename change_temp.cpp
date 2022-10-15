#include <iostream>

int main(){
  std::cout << "keisuu: ";
  double p;
  std::cin >> p;
  std::string s_single = "#include \"audio/wave.hpp\"\n";
  s_single += "constexpr double t0 = 2.5e3 * analyze_sampling_hz / 6000.0 * ";
  s_single += std::to_string(p) + ";\n";
  s_single += "constexpr double t1 = 1.0e2 * analyze_sampling_hz / 6000.0 * ";
  s_single += std::to_string(p) + ";\n";

  std::string s_multi = "#include \"audio/wave.hpp\"\n";
  s_multi += "constexpr double t0 = 2.5e3 * analyze_sampling_hz / 6000.0 * 0.9 * ";
  s_multi += std::to_string(p) + ";\n";
  s_multi += "constexpr double t1 = 1.0e2 * analyze_sampling_hz / 6000.0 * 0.9 * ";
  s_multi += std::to_string(p) + ";\n";
  FILE *fp = fopen("analyze/temp.hpp", "w");
  if(fp == NULL){
    puts("temp.hpp was not found.");
    return 0;
  }
  fwrite(s_single.c_str(), 1, s_single.size(), fp);
  fclose(fp);
  fp = fopen("analyze/temp_thread.hpp", "w");
  if(fp == NULL){
    puts("temp_thread.hpp was not found.");
    return 0;
  }
  fwrite(s_multi.c_str(), 1, s_multi.size(), fp);
  fclose(fp);
}