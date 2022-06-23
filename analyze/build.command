#!/bin/bash
cd `dirname $0`
g++ generator.cpp -O2 -o generator
./generator test 20 1 8
g++ tester.cpp -O2 -o tester
./tester test > in.txt
g++ create_audio_array.cpp -O2
./a.out
g++ yakinamashi.cpp -O3
