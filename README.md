# procon33

## analyze

### yakinamashi.cpp
 - シングルスレッドで音声の解析をする
 - Compile: $ g++ yakinamashi.cpp -Ofast -fopenmp -lgomp

### yakinamashi_thread.cpp
 - マルチスレッドでの音声の解析をする(pthread使います)
 - Compile: $ g++ yakinamashi_thread.cpp -Ofast -fopenmp -lgomp

### generator.cpp
 - ランダムで問題の作成をする
 - Compile: $ g++ generator.cpp -O2 -o generator
 - Run: $ ./generator test [札の数] [分割数] [音声の最大長(s)]

### run.cpp
 - testディレクトリにあるinformation.txtとproblem.wavを使って問題を解く
 - Compile: $ g++ run.cpp -O2 -o run
 - Run: $ ./run

### train.cpp
 - run.cppを複数回動かしてテストする
 - Compile: $ g++ train.cpp -O2 -o train
 - Run: $ ./train [書き込むファイルのパス] [テスト回数]
