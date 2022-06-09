#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <direct.h>
#include "wave.hpp"
#define rep(i, n) for(int i = 0; i < (n); i++)
using std::vector;
using std::cin; using std::cerr;
using std::cerr;
using uint = unsigned int;
using ll = long long;
enum { JA, EN };

void write_text(const char *txt, const char *filename){
  FILE *fp = fopen(filename, "w");
  assert(fp != NULL);

  fwrite(txt, 1, strlen(txt), fp);
  fclose(fp);
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
