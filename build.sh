#! /bin/sh
cd analyze
g++ run.cpp -O2 -o run
cd ..
g++ operator.cpp -O2 -o operator
g++ transform.cpp -o transform