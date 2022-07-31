#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <utility>
#include "genann.hpp"
#include "../audio/wave.hpp"
#include "SvgDrawer.hpp"
using ll = long long;

std::vector<std::pair<int,int>> calc_silent_wave(const Wave &wave){
  std::vector<int> isok(wave.L);
  std::vector<double> val(wave.L);
  for(int i = 0; i < wave.L; i++){
    ll tot = 0;
    int mx = 0;
    constexpr int l = 50;
    for(int j = -std::min(i,l); j < l && j+i < wave.L; j++){
      tot += (ll)wave[i+j] * wave[i+j];
      mx = std::max(mx, abs(wave[i+j]));
    }
    val[i] = sqrt((double)tot / l/2);
    if(mx < 80 || val[i] < 32) isok[i] = 1;
  }
  for(int i = 0; i < wave.L; i++){
    int cnt = 0;
    for(int j = std::max(-i, -2); j <= std::min(wave.L-i-1,2); j++){
      cnt += isok[i + j];
    }
    if(cnt >= 3 && val[i] < 60){
      isok[i] = 1;
    }
  }
  std::vector<std::pair<int,int>> res;
  int last = 0;
  for(int i = 1; i < wave.L - 1; i++) if(isok[i]){
    if(!isok[i-1]){
      last = i;
    }
    if(!isok[i+1]){
      res.push_back({ last, i });
    }
  }
  return res;
}


int main(int argc, char *argv[]){
  // srand(time(NULL));

  // /* Input and expected out data for the XOR function. */
  // const double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
  // const double output[4] = {0, 1, 1, 0};

  // /* New network with 2 inputs,
  //   * 1 hidden layer of 2 neurons,
  //   * and 1 output. */
  // genann *ann = genann_init(2, 1, 4, 1);

  // /* Train on the four labeled data points many times. */
  // for(int i = 0; i < 500*500; i++){
  //   genann_train(ann, input[0], output + 0, 3);
  //   genann_train(ann, input[1], output + 1, 3);
  //   genann_train(ann, input[2], output + 2, 3);
  //   genann_train(ann, input[3], output + 3, 3);
  // }

  // /* Run the network and see what it predicts. */
  // printf("Output for [%1.f, %1.f] is %1.5f.\n", input[0][0], input[0][1], *genann_run(ann, input[0]));
  // printf("Output for [%1.f, %1.f] is %1.5f.\n", input[1][0], input[1][1], *genann_run(ann, input[1]));
  // printf("Output for [%1.f, %1.f] is %1.5f.\n", input[2][0], input[2][1], *genann_run(ann, input[2]));
  // printf("Output for [%1.f, %1.f] is %1.5f.\n", input[3][0], input[3][1], *genann_run(ann, input[3]));

  // genann_free(ann);
}