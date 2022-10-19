#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <time.h>
#include <cstring>
#include <chrono>
#include <omp.h>
#include "audio_array.hpp"
#include "Math.hpp"
#include "hash.hpp"
#include "select_num.hpp"
#if defined(_WIN32) || defined(_WIN64)
#pragma GCC target("avx2")
#endif
#pragma GCC optimize("unroll-loops")
#define rep(i, n) for(int i = 0; i < (n); i++)
using uint = unsigned int;

constexpr int inf = (uint)-1 >> 1;

constexpr int n = 44*2; //candidate arrays
constexpr int half_n = n / 2;
constexpr int hz = analyze_sampling_hz; //sampling hz[48k->12k]
constexpr int tot_frame = analyze_audio_max_length; //max size of arrays[i]
constexpr int ans_length = hz * 16;

// 数列の値の型
using Val_Type = int;
using Score_Type = int;
constexpr Score_Type inf_score = (1ULL << (sizeof(Score_Type)*8-1)) - 1;

int audio_length[n] = {};
Val_Type problem[ans_length] = {};
int problem_length = ans_length;
int contains_num = 0; //確実に含まれている札の数

struct Data {
  int idx; //札の種類
  int st; //札の再生開始位置
};
Data answer[m];

// returns random [l, r)
inline int rnd(const int l, const int r) noexcept{
  static uint x = (uint)rand() | (uint)rand() << 16;
  x ^= x << 13; x ^= x >> 17;
  return (int)((x ^= x << 5) % (uint)(r - l)) + l;
}
// returns random [0, rng)
inline int rnd(const int rng) noexcept{
  static uint x = (uint)rand() | (uint)rand() << 16;
  x ^= x << 13; x ^= x >> 17;
  return (x ^= x << 5) % (uint)rng;
}

inline constexpr Val_Type Weight(const Val_Type x) noexcept{
  return x < 0 ? -x : x;
}


constexpr Score_Type calc_score(const Val_Type a[ans_length]) noexcept{
  Score_Type score = 0;
  rep(i, problem_length) score += Weight(a[i]);
  return score;
}


namespace Solver {
  void init();
  void init_values(const Data pre_result[m], const int contain);
} // namespace Solver

namespace File {

void read_values(){
  rep(i, n){
    audio_length[i] = tot_frame;
    rep(j, tot_frame){
      if(arrays[i][j] == inf){
        audio_length[i] = j;
        break;
      }
    }
  }
  Wave wave_data;
  read_audio(wave_data, "test/problem.wav");
  change_sampling_hz(wave_data, analyze_sampling_hz);
  assert(wave_data.L <= ans_length);
  for(int i = 0; i < wave_data.L; i++) problem[i] = wave_data[i];
  problem_length = wave_data.L;
  
  std::string s;
  std::cin >> s;
  rep(i, m) answer[i].idx = -1;
  if(s == "Input"){
    Solver::init();
  }else if(s == "Output"){
    Data pre_values[m];
    int tmp;
    int contain;
    rep(i, m) std::cin >> tmp; // fuda
    std::cin >> tmp; // score
    std::cin >> contain; // contains num
    // data
    rep(i, m){
      std::cin >> pre_values[i].idx >> pre_values[i].st;
      pre_values[i].st *= analyze_change_prop;
    }
    Solver::init_values(pre_values, contain);
  }else{
    std::cerr << "wrong format!!!\n";
    assert(0);
  }
}

void output_result(const Data best[m], const Score_Type final_score){
  std::cout << "Output\n";
  rep(i, m) std::cout << best[i].idx << " \n"[i == m - 1];
  std::cout << final_score << "\n";
  std::cout << "\n";
  std::cout << contains_num << "\n";
  rep(i, m){
    std::cout << best[i].idx << " " << best[i].st << "\n";
  }
}

}; // namespace File

namespace Solver {

struct RndInfo {
  int idx; //変更する値
  int nxt_idx; //新しく更新する札
  int st; //次の札の再生開始位置
};

Data best[m];
uint64_t used_idx = 0;
Val_Type best_sub[ans_length];

Score_Type problem_wave_score = inf_score;
Score_Type best_score = inf_score;

void init(){
  memcpy(best_sub, problem, sizeof(problem));
  problem_wave_score = calc_score(best_sub);
  contains_num = 0;
  // 最初はランダムに値を入れておく
  for(int i = contains_num; i < m; i++){
    int idx = rnd(n);
    while(used_idx >> (idx % half_n) & 1){
      idx = rnd(n);
    }
    best[i].idx = idx;
    best[i].st = 0;
    used_idx |= 1ULL << (best[i].idx % half_n);
  }
  rep(i, m){
    rep(j, min(audio_length[best[i].idx], problem_length)){
      best_sub[j] -= arrays[best[i].idx][j + best[i].st];
    }
  }
  best_score = calc_score(best_sub);

  std::cerr << "Problem Wave: " << problem_wave_score << "\n";
  std::cerr << "First Score: " << best_score << "\n";
}
void init_values(const Data pre_result[m], const int contain){
  memcpy(best_sub, problem, sizeof(problem));
  problem_wave_score = calc_score(best_sub);
  contains_num = contain;
  memcpy(best, pre_result, sizeof(best));
  rep(i, m){
    used_idx |= 1ULL << (best[i].idx % half_n);
    rep(j, min(audio_length[best[i].idx], problem_length)){
      best_sub[j] -= arrays[best[i].idx][j + best[i].st];
    }
  }
  best_score = calc_score(best_sub);

  std::cerr << "Problem Wave: " << problem_wave_score << "\n";
  std::cerr << "First Score: " << best_score << "\n";
}

// import to best from data
void init_array(const Data data[]) noexcept{
  memcpy(best, data, sizeof(best));
  used_idx = 0;
  memcpy(best_sub, problem, sizeof(problem));
  rep(i, m){
    used_idx |= 1ULL << (best[i].idx % half_n);
    rep(j, min(audio_length[best[i].idx], problem_length)){
      best_sub[j] -= arrays[best[i].idx][j + best[i].st];
    }
  }
}


inline void rnd_create(RndInfo &change) noexcept{
  static constexpr int rng = hz / 8;
  const int t = rnd(7);
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.st = rnd(audio_length[change.nxt_idx] - problem_length + 1);
  }
  // select other wav and swap and change pos
  else if(t == 1 || t == 2){
    change.idx = rnd(contains_num, m);
    change.nxt_idx = rnd(n);
    while((used_idx >> (change.nxt_idx % half_n) & 1) && best[change.idx].idx % half_n != change.nxt_idx % half_n){
      change.nxt_idx = rnd(n);
    }
    change.st = rnd(audio_length[change.nxt_idx] - problem_length + 1);
  }
  // change st
  else if(t == 3){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.st = rnd(max(0, best[change.idx].st-rng), min(audio_length[change.nxt_idx]-problem_length, best[change.idx].st+rng) + 1);
  }
  // change wav type
  else if(t == 4 || t == 5){
    change.idx = rnd(contains_num, m);
    change.nxt_idx = rnd(n);
    while((used_idx >> (change.nxt_idx % half_n) & 1) && best[change.idx].idx % half_n != change.nxt_idx % half_n){
      change.nxt_idx = rnd(n);
    }
    change.st = min(best[change.idx].st, audio_length[change.nxt_idx] - problem_length);
  }
  // copy other wav info
  else if(t == 6){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    int sel_idx = rnd(m);
    while(sel_idx != change.idx) sel_idx = rnd(m);
    change.st = min(audio_length[change.nxt_idx] - problem_length, best[sel_idx].st);
  }
}

inline Score_Type calc_one_changed_ans(const RndInfo &info) noexcept{
  const Data &pre = best[info.idx];
  const Val_Type *arr_info = arrays[info.nxt_idx] + info.st;
  const Val_Type *arr_pre = arrays[pre.idx] + pre.st;
  Score_Type score = 0;
  rep(i, problem_length){
    score += Weight(best_sub[i] - arr_info[i] + arr_pre[i]);
  }
  return score;
}


inline void update_values(const RndInfo &info) noexcept{
  // update best_sub
  const Data &pre = best[info.idx];
  rep(i, problem_length){
    best_sub[i] += arrays[pre.idx][i + pre.st] - arrays[info.nxt_idx][i + info.st];
  }
  // update info
  used_idx ^= 1ULL << (best[info.idx].idx % half_n);
  used_idx ^= 1ULL << (info.nxt_idx % half_n);
  best[info.idx].idx = info.nxt_idx;
  best[info.idx].st = info.st;
}

}; // namespace solver


struct StopWatch {
  const std::chrono::system_clock::time_point start_time;
  StopWatch() : start_time(std::chrono::system_clock::now()){}
  inline double get_time() const noexcept{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time).count() * 1e-6;
  }
};

struct ios_do_not_sync {
  ios_do_not_sync(){
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
  }
} ios_do_not_sync_instance;