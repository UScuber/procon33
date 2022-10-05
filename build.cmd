@echo off
cd analyze
g++ run.cpp -O2 -o run.exe
cd ..
g++ operator.cpp -O2 -o operator.exe