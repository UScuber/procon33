g++ generator.cpp -O2 -o generator.exe
generator.exe test %1 %2 %3
g++ tester.cpp -O2 -o tester.exe
tester.exe test > in.txt
g++ create_audio_array.cpp -O2
a.exe
