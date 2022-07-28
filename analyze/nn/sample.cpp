#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include "genann.hpp"
#include "../audio/wave.hpp"
#include "SvgDrawer.hpp"
using ll = long long;

int main(int argc, char *argv[]){
  Wave wave;
  read_audio(wave, "../audio/JKspeech/E02.wav");
  change_sampling_hz(wave, 48000/2);
  write_audio(wave, "out.wav");

  mi::SvgDrawer drawer(1600, 300, "test.svg");
	drawer.setStrokeColor("blue");
  drawer.setViewBox(-100, -20000, wave.L, 20000);
  for(int i = 0; i < wave.L; i++){
    bool ok = true;
    ll tot = 0;
    constexpr int l = 50/2;
    for(int j = -std::min(i,l); j < l && j+i < wave.L; j++){
      tot += (ll)wave[i+j] * wave[i+j];
    }
    if(sqrt((double)tot / l/2) < 20);
    else ok = false;
    if(ok){
      drawer.setStrokeColor("red");
      drawer.drawLine(i, 0, i, 3000);
      drawer.setStrokeColor("blue");
    }
    else drawer.drawLine(i, 0, i, wave[i]);
  }
	/*
  drawer.setStrokeColor("red");
	drawer.drawLine(-1.5, -1.5, -1.5, 1.5);
	drawer.setStrokeColor("blue");
	drawer.drawLine(-1.5, 1.5, 1.5, 1.5);
	drawer.setStrokeColor("yellow");
	drawer.drawLine(1.5, 1.5, 1.5, -1.5);
	drawer.setStrokeColor("green");
	drawer.drawLine(1.5, -1.5, -1.5, -1.5);
  */
  // printf("GENANN example 1.\n");
  // printf("Train a small ANN to the XOR function using backpropagation.\n");

  // /* This will make the neural network initialize differently each run. */
  // /* If you don't get a good result, try again for a different result. */
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