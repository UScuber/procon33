#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <cassert>
#include <time.h>
#include <direct.h>
#define rep(i, n) for(int i = 0; i < (n); i++)
using std::vector;
using std::cin; using std::cerr;
using std::cerr;
using uint = unsigned int;
using ll = long long;
constexpr int short_max = (1 << 15) - 1;
enum { JA, EN };

//URL: https://hwswsgps.hatenablog.com/entry/2018/08/19/172401
struct Wave {
  int fs; //サンプリング周波数
  int bits; //量子化bit数
  int L = 0; //データ長
  vector<int> data;
  int &operator[](const int &i) noexcept{
    assert(0 <= i && i < L);
    return data[i];
  }
  const int &operator[](const int &i) const noexcept{
    assert(0 <= i && i < L);
    return data[i];
  }
};

void read_audio(Wave &prm, const char *filename){
  char tmp[24];

  long fmt_samples_per_sec;
  short fmt_bits_per_sample;
  long data_size;

  //wavファイルオープン
  FILE *fp = fopen(filename, "rb");
  assert(fp != NULL);

  //wavデータ読み込み
  fread(tmp, 1, 24, fp);
  fread(&fmt_samples_per_sec, 4, 1, fp);
  fread(tmp, 1, 6, fp);
  fread(&fmt_bits_per_sample, 2, 1, fp);
  fread(tmp, 1, 4, fp);
  fread(&data_size, 4, 1, fp);

  //パラメータ代入
  prm.fs = fmt_samples_per_sec;
  prm.bits = fmt_bits_per_sample;
  prm.L = data_size / 2;

  //音声データ代入
  prm.data.resize(prm.L);
  for(int i = 0; i < prm.L; i++){
    short data_data;
    fread(&data_data, 2, 1, fp);
    prm[i] = data_data;
  }
  fclose(fp);
}
void write_audio(const Wave &prm, const char *filename){
  FILE *fp = fopen(filename, "wb");
  assert(fp != NULL);

  //ヘッダー書き込み
  const long header_size = 36 + prm.L * 2;

  fwrite("RIFF", 1, 4, fp);
  fwrite(&header_size, 4, 1, fp);
  fwrite("WAVE", 1, 4, fp);
  fwrite("fmt ", 1, 4, fp);

  const long fmt_size = 16;
  const short fmt_format = 1;
  const short fmt_channel = 1;
  const long fmt_samples_per_sec = prm.fs;
  const long fmt_bytes_per_sec = prm.fs * prm.bits / 8;
  const short fmt_block_size = prm.bits / 8;
  const short fmt_bits_per_sample = prm.bits;
  fwrite(&fmt_size, 4, 1, fp);
  fwrite(&fmt_format, 2, 1, fp);
  fwrite(&fmt_channel, 2, 1, fp);
  fwrite(&fmt_samples_per_sec, 4, 1, fp);
  fwrite(&fmt_bytes_per_sec, 4, 1, fp);
  fwrite(&fmt_block_size, 2, 1, fp);
  fwrite(&fmt_bits_per_sample, 2, 1, fp);

  //データ書き込み
  const long data_size = prm.L * 2;
  fwrite("data", 1, 4, fp);
  fwrite(&data_size, 4, 1, fp);

  //音声データ書き込み
  for(int i = 0; i < prm.L; i++){
    short data_data;
    //リミッター
    if(prm[i] > short_max) data_data = short_max;
    else if(prm[i] < -short_max) data_data = -short_max;
    else data_data = prm[i];
    fwrite(&data_data, 2, 1, fp);
  }
  fclose(fp);
}

void write_text(const char *txt, const char *filename){
  FILE *fp = fopen(filename, "w");
  assert(fp != NULL);

  fwrite(txt, 1, strlen(txt), fp);
  fclose(fp);
}
//prm.fs[Hz] -> fs[Hz]
void change_sampling_hz(Wave &prm, int fs){
  assert(prm.fs % fs == 0);
  const int p = prm.fs / fs;
  prm.fs = fs;
  prm.L /= p;
  vector<int> new_data(prm.L);
  rep(i, prm.L){
    int tot = 0;
    //rep(j, p) tot += prm[i*p + j];
    //new_data[i] = tot / p;
    new_data[i] = prm[i * p];
  }
  prm.data = std::move(new_data);
}

// st[i]: waves[i]の開始位置(1sample単位)
[[nodiscard]]
Wave merge_audio(const vector<Wave> &waves, const vector<int> &st){
  assert(waves.size() == st.size());
  Wave res;
  res.bits = waves[0].bits;
  res.fs = waves[0].fs;
  res.L = 0;
  const int n = waves.size();
  for(int i = 0; i < n; i++){
    assert(res.bits == waves[i].bits && res.fs == waves[i].fs);
    res.L = std::max(res.L, waves[i].L + st[i]);
  }
  res.data.assign(res.L, 0);
  for(int i = 0; i < n; i++){
    for(int j = 0; j < waves[i].L; j++){
      res[j + st[i]] += waves[i][j];
    }
  }
  for(int i = 0; i < res.L; i++){
    if(res[i] > short_max) res[i] = short_max;
    else if(res[i] < -short_max) res[i] = -short_max;
  }
  return res;
}

//sep[i]: 分割する位置。sep.front() = 0, sep.back() = wave.L
[[nodiscard]]
vector<Wave> separate_audio(const Wave &wave, const vector<int> &sep){
  const int n = sep.size();
  assert(sep[0] == 0 && sep[n - 1] == wave.L);
  for(int i = 1; i < n - 1; i++){
    assert(sep[i] < sep[i + 1]);
    assert(0 < sep[i] && sep[i] < wave.L);
  }
  vector<Wave> res(n - 1);
  for(int i = 0; i < n - 1; i++){
    res[i].bits = wave.bits;
    res[i].fs = wave.fs;
    res[i].L = sep[i + 1] - sep[i];
    for(int j = sep[i]; j < sep[i + 1]; j++){
      res[i].data.emplace_back(wave[j]);
    }
    assert(res[i].data.size() == sep[i + 1] - sep[i]);
  }
  return res;
}


inline uint randxor() noexcept{
  static uint x = rand() | rand() << 16;
  static uint y = rand() | rand() << 16;
  static uint z = rand() | rand() << 16;
  static uint w = rand() | rand() << 16;
  //static uint x=123456789,y=362436069,z=521288629,w=88675123;
  uint t=(x^(x<<11));x=y;y=z;z=w; return (w=(w^(w>>19))^(t^(t>>8)));
}
// returns random [l, r)
inline int rnd(const int &l, const int &r) noexcept{
  return randxor() % (r - l) + l;
}

constexpr int n = 44;
constexpr int hz = 48000;
int audio_num[n];
int m, sep_num;
double time_limit;
vector<Wave> data;

//generator.exe [export directory]
int main(int argc, char *args[]){
  if(argc != 2){
    cerr << "arguments are too few\n";
    return 1;
  }
  srand(time(NULL));
  cerr << "how many audios & separates & max time? ";
  cin >> m >> sep_num >> time_limit;
  _mkdir(args[1]);
  int max_time = std::max(time_limit, sep_num*0.5) * hz;
  data.resize(m);
  vector<std::pair<int,int>> info(m);

  // read random audio
  cerr << "read random audio\n";
  ll used = 0;
  char buf[64];
  rep(i, m){
    int val = rnd(0, n);
    while(used >> val & 1) val = rnd(0, n);
    const int lang = rnd(0, 2);
    used |= 1LL << val;
    audio_num[i] = val;
    sprintf(buf, "./JKspeech/%c%02d.wav", (lang == JA ? 'J' : 'E'), val + 1);
    read_audio(data[i], buf);
    info[i] = { val, lang };
  }

  // clip random
  cerr << "clip random\n";
  rep(i, m){
    // clip [l, r)
    const int l = rnd(0, data[i].L - hz);
    const int r = rnd(l + hz, data[i].L) + 1;
    assert(0 <= l && l < r && r < data[i].L);
    vector<int> sep{ 0, l, r, data[i].L };
    if(l == 0) sep.erase(sep.begin());
    if(r == data[i].L) sep.pop_back();
    const vector<Wave> temp = separate_audio(data[i], sep);
    if(l == 0) data[i] = temp[0];
    else data[i] = temp[1];
  }

  // create problem file
  cerr << "create problem file\n";
  rep(i, m) max_time = std::max(max_time, data[i].L);
  vector<int> st(m);
  int last = -1;
  used = 0;
  rep(i, m){
    int p = rnd(0, m);
    while(used >> p & 1) p = rnd(0, m);
    used |= 1LL << p;
    if(!i) st[p] = 0;
    else{
      if(st[last] + data[p].L > max_time) st[p] = rnd(0, max_time - data[p].L + 1);
      else st[p] = rnd(st[last], std::min(max_time - data[p].L + 1, st[last] + data[last].L + 1));
    }
    last = p;
  }
  Wave problem = merge_audio(data, st);

  // separate random
  cerr << "separate random\n";
  vector<int> sep{ 0 };
  rep(i, sep_num - 1){
    int pos = rnd(sep[i] + hz/2, problem.L) + 1;
    while(problem.L - pos < hz/2 * (sep_num-i-1)) pos = rnd(sep[i] + hz/2, problem.L) + 1;
    sep.push_back(pos);
  }
  assert(sep.back() < problem.L);
  sep.push_back(problem.L);
  const vector<Wave> sep_audios = separate_audio(problem, sep);
  assert(sep_audios.size() == sep_num);

  // output problem audios
  cerr << "output problem audios\n";
  sprintf(buf, "./%s/problem.wav", args[1]);
  write_audio(problem, buf);
  rep(i, sep_num){
    sprintf(buf, "./%s/sep%d.wav", args[1], i + 1);
    write_audio(sep_audios[i], buf);
  }
  for(int i = sep_num; ; i++){
    sprintf(buf, "./%s/sep%d.wav", args[1], i + 1);
    if(remove(buf)) break;
  }

  // output problem information
  std::string answer = "";
  answer += "n speech: " + std::to_string(m) + "\n\n";
  answer += "speech: ";
  rep(i, m){
    sprintf(buf, "%c%02d", (info[i].second == JA ? 'J' : 'E'), info[i].first + 1);
    answer += buf;
    answer += ",\n"[i == m - 1];
  }
  answer += "\noffset: ";
  rep(i, m){
    answer += std::to_string(st[i]);
    answer += ",\n"[i == m - 1];
  }
  answer += "\nn split: " + std::to_string(sep_num) + "\n\n";
  answer += "duration: ";
  rep(i, sep_num){
    answer += std::to_string(sep_audios[i].L);
    answer += ",\n"[i == sep_num - 1];
  }
  sprintf(buf, "./%s/information.txt", args[1]);
  write_text(answer.c_str(), buf);
}
