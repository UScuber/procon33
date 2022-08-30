#!/bin/bash
cd `dirname $0`
g++ generator.cpp -O2 -o generator
./generator test 20 1 8
g++ tester.cpp -O2 -o tester
./tester test > in.txt
g++ yakinamashi.cpp -O3
