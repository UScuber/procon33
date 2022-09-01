#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <time.h>
#include <cstring>
#include <chrono>
#include "Math.hpp"
#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#include "audio_array.hpp"
#define rep(i, n) for(int i = 0; i < (n); i++)
using uint = unsigned int;
using std::vector;
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
using std::cout; using std::cin;
using std::cerr;

constexpr int inf = (uint)-1 >> 1;

constexpr int n = 44*2; //candidate arrays
constexpr int half_n = n / 2;
constexpr int m = 20; //select num
constexpr int hz = analyze_sampling_hz; //sampling hz[48k->12k]
constexpr int tot_frame = analyze_audio_max_length; //max size of arrays[i]
constexpr int ans_length = hz * 8;
static_assert(m <= half_n);

// 数列の値の型
using Val_Type = int;
using Score_Type = int;
constexpr Score_Type inf_score = (1ULL << (sizeof(Score_Type)*8-1)) - 1;

int audio_length[n] = {};
Val_Type problem[ans_length] = {};
int problem_length = ans_length;
bool has_answer = true;

struct Data {
  int idx; //札の種類
  int pos; //貼り付け位置
  int st; //札の再生開始位置
  int len; //札の再生の長さ
};
Data answer[m];

// returns random [l, r)
inline int rnd(const int l, const int r) noexcept{
  static uint x = rand() | rand() << 16;
  x ^= x << 13; x ^= x >> 17;
  return (int)((x ^= x << 5) % (uint)(r - l)) + l;
}
// returns random [0, rng)
inline int rnd(const int rng) noexcept{
  static uint x = rand() | rand() << 16;
  x ^= x << 13; x ^= x >> 17;
  return (x ^= x << 5) % (uint)rng;
}

inline constexpr Val_Type Weight(const Val_Type x) noexcept{
  return x < 0 ? -x : x;
}
//#define Weight(x) abs(x)
// check is crossed [a, b), [c, d)
#define is_cross(a,b,c,d) (max(a, c) < min(b, d))

// 問題の数列から数字を引いたやつのスコアを計算する
constexpr Score_Type calc_score(const Val_Type a[ans_length]) noexcept{
  Score_Type score = 0;
  rep(i, problem_length) score += Weight(a[i]);
  return score;
}


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
  rep(i, ans_length){
    std::cin >> problem[i];
    if(problem[i] == inf){
      problem_length = i;
      problem[i] = 0;
      break;
    }
  }
  rep(i, m) std::cin >> answer[i].idx;
  rep(i, m) std::cin >> answer[i].pos;
  rep(i, m) std::cin >> answer[i].len;
  rep(i, m) std::cin >> answer[i].st;
  if(std::cin.eof()) has_answer = false;

  // output answer_idx
  /*
  if(has_answer) rep(i, m){
    if(answer[i].idx < half_n) cout << "J" << answer[i].idx+1;
    else cout << "E" << answer[i].idx-half_n+1;
    cout << "\n";
  }
  cout << "\n";
  */
}

void output_result(const Data best[m]){
  /*
  rep(i, m){
    if(best[i].idx < half_n) cout << "J" << best[i].idx+1;
    else cout << "E" << best[i].idx-half_n+1;
    cout << " " << best[i].pos * 4 << "\n";
  }
  */

  if(has_answer){
    int audio_diff_num = 0;
    int karuta_diff_num = 0;
    rep(i, m){
      bool ok = false, ok2 = false;
      rep(j, m){
        if(best[i].idx == answer[j].idx){
          ok = ok2 = true;
          break;
        }else if(best[i].idx % half_n == answer[j].idx % half_n){
          ok2 = true;
        }
      }
      if(!ok){
        audio_diff_num++;
        /*
        if(best[i].idx < half_n) cout << "J" << best[i].idx+1;
        else cout << "E" << best[i].idx-half_n+1;
        cout << " ";
        */
      }
      if(!ok2) karuta_diff_num++;
    }
    cerr << "Audio Diff: " << audio_diff_num << "/" << m << "\n";
    cerr << "Karuta Diff: " << karuta_diff_num << "/" << m << "\n";
    cout << audio_diff_num << " " << karuta_diff_num << "\n";
  }
  cout << "\n";
  rep(i, m){
    cout << "Fuda: " << best[i].idx << ", Pos: " << best[i].pos << ", St: " << best[i].st << ", Len: " << best[i].len << "\n";
  }
}

}; // namespace File


namespace Solver {

struct RndInfo {
  int idx; //変更する値
  int pos; //変更後の貼り付け位置
  int nxt_idx; //新しく更新する札
  int st; //次の札の再生開始位置
  int len; //次の札の再生の長さ
};

Data best[m];
uint64_t used_idx = 0;
Val_Type best_sub[ans_length];

Score_Type best_score = inf_score;

void init(){
  memcpy(best_sub, problem, sizeof(problem));
  // 最初は適当に値を入れておく
  rep(i, m){
    best[i].idx = i;
    best[i].pos = 0;
    best[i].st = 0;
    best[i].len = min(problem_length, audio_length[best[i].idx]);
    // best[i].idx = answer[i].idx;
    // best[i].pos = answer[i].pos / (default_sampling_hz / analyze_sampling_hz);
    // best[i].st = answer[i].st / (default_sampling_hz / analyze_sampling_hz);
    // best[i].len = answer[i].len / (default_sampling_hz / analyze_sampling_hz);
    used_idx |= 1ULL << best[i].idx;
    // best_subの計算
    rep(j, best[i].len){
      best_sub[j + best[i].pos] -= arrays[best[i].idx][j + best[i].st];
    }
  }
  best_score = calc_score(best_sub);

  cerr << "First Score: " << best_score << "\n";
}

// import to best from data
void init_array(const Data data[]) noexcept{
  memcpy(best, data, sizeof(best));
  used_idx = 0;
  memcpy(best_sub, problem, sizeof(problem));
  rep(i, m){
    used_idx |= 1ULL << (best[i].idx % half_n);
    rep(j, best[i].len){
      best_sub[j + best[i].pos] -= arrays[best[i].idx][j + best[i].st];
    }
  }
}


inline void rnd_create(RndInfo &change) noexcept{
  constexpr int rng = hz;
  const int t = rnd(7);
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(problem_length - change.len + 1);
  }
  // select other wav and swap and change pos
  else if(t == 1 || t == 6){
    change.idx = rnd(m);
    change.nxt_idx = rnd(n);
    while((used_idx >> (change.nxt_idx % half_n) & 1) && best[change.idx].idx % half_n != change.nxt_idx % half_n){
      change.nxt_idx = rnd(n);
    }
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(problem_length - change.len + 1);
  }
  // change len
  else if(t == 2){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.st = best[change.idx].st;
    change.pos = best[change.idx].pos;
    //change.len = rnd(hz, min(problem_length-change.pos, audio_length[change.nxt_idx]-change.st) + 1);
    change.len = rnd(max(hz, best[change.idx].len-rng), min(min(problem_length-change.pos, audio_length[change.nxt_idx]-change.st), best[change.idx].len+rng) + 1);
  }
  // change pos
  else if(t == 3){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.st = best[change.idx].st;
    change.len = best[change.idx].len;
    //change.pos = rnd(problem_length - change.len + 1);
    change.pos = rnd(max(0, best[change.idx].pos-rng), min(problem_length-change.len, best[change.idx].pos+rng) + 1);
  }
  // change st
  else if(t == 4){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.len = best[change.idx].len;
    change.pos = best[change.idx].pos;
    //change.st = rnd(min(audio_length[change.nxt_idx] - change.len, problem_length-change.pos) + 1);
    change.st = rnd(max(0, best[change.idx].st-rng), min(audio_length[change.nxt_idx]-change.len, best[change.idx].st+rng) + 1);
  }
  // change wav type
  else if(t == 5){
    change.idx = rnd(m);
    change.nxt_idx = rnd(n);
    while((used_idx >> (change.nxt_idx % half_n) & 1) && best[change.idx].idx % half_n != change.nxt_idx % half_n){
      change.nxt_idx = rnd(n);
    }
    change.len = min(best[change.idx].len, audio_length[change.nxt_idx]);
    change.st = min(best[change.idx].st, audio_length[change.nxt_idx] - change.len);
    change.pos = min(best[change.idx].pos, problem_length - change.len);
  }
}
inline void rnd_create2(RndInfo &change) noexcept{
  const int t = rnd(4);
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(problem_length - change.len + 1);
  }
  // select other wav and swap and change pos
  else if(t == 1 || t == 3){
    change.idx = rnd(m);
    change.nxt_idx = rnd(n);
    while((used_idx >> (change.nxt_idx % half_n) & 1) && best[change.idx].idx % half_n != change.nxt_idx % half_n){
      change.nxt_idx = rnd(n);
    }
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(problem_length - change.len + 1);
  }
  // change wav type
  else if(t == 2){
    change.idx = rnd(m);
    change.nxt_idx = rnd(n);
    while((used_idx >> (change.nxt_idx % half_n) & 1) && best[change.idx].idx % half_n != change.nxt_idx % half_n){
      change.nxt_idx = rnd(n);
    }
    change.len = min(best[change.idx].len, audio_length[change.nxt_idx]);
    change.st = min(best[change.idx].st, audio_length[change.nxt_idx] - change.len);
    change.pos = min(best[change.idx].pos, problem_length - change.len);
  }
}

inline Score_Type calc_one_changed_ans(const RndInfo &info) noexcept{
  const Data &pre = best[info.idx];
  const int info_rig = info.pos + info.len;
  const int pre_rig = pre.pos + pre.len;
  const Val_Type *arr_info = arrays[info.nxt_idx] + info.st;
  const Val_Type *arr_pre = arrays[pre.idx] + pre.st;
  Score_Type score = best_score;
  // cross
  if(is_cross(info.pos, info_rig, pre.pos, pre_rig)){
    // 左側がleft
    if(info.pos <= pre.pos){
      const int rightest = min(info_rig, pre.pos);
      for(int i = info.pos; i < rightest; i++){
        score += Weight(best_sub[i] - arr_info[i-info.pos]) - Weight(best_sub[i]);
      }
    }
    // 左側がright
    else{
      const int rightest = min(pre_rig, info.pos);
      for(int i = pre.pos; i < rightest; i++){
        score += Weight(best_sub[i] + arr_pre[i-pre.pos]) - Weight(best_sub[i]);
      }
    }
    // 右側がright
    if(info_rig <= pre_rig){
      const int leftest = max(pre.pos, info_rig);
      const int s = max(info_rig - pre.pos, 0);
      for(int i = leftest; i < pre_rig; i++){
        score += Weight(best_sub[i] + arr_pre[i+s-leftest]) - Weight(best_sub[i]);
      }
    }
    // 右側がleft
    else{
      const int leftest = max(info.pos, pre_rig);
      const int s = max(pre_rig - info.pos, 0);
      for(int i = leftest; i < info_rig; i++){
        score += Weight(best_sub[i] - arr_info[i+s-leftest]) - Weight(best_sub[i]);
      }
    }
    // middle
    const int leftest = max(info.pos, pre.pos);
    const int range = min(info_rig, pre_rig) - leftest;
    const int sl = max(pre.pos - info.pos, 0);
    const int sr = max(info.pos - pre.pos, 0);
    rep(i, range){
      score -= Weight(best_sub[i+leftest]);
      score += Weight(best_sub[i+leftest] - arr_info[i+sl] + arr_pre[i+sr]);
    }
  }
  // not cross
  else{
    for(int i = 0; i < info.len; i++){
      score += Weight(best_sub[i+info.pos] - arr_info[i]) - Weight(best_sub[i+info.pos]);
    }
    for(int i = 0; i < pre.len; i++){
      score += Weight(best_sub[i+pre.pos] + arr_pre[i]) - Weight(best_sub[i+pre.pos]);
    }
  }
  return score;
}


inline void update_values(const RndInfo &info) noexcept{
  // update best_sub
  const Data &pre = best[info.idx];
  rep(i, pre.len){
    best_sub[i + pre.pos] += arrays[pre.idx][i + pre.st];
  }
  rep(i, info.len){
    best_sub[i + info.pos] -= arrays[info.nxt_idx][i + info.st];
  }
  // update info
  used_idx ^= 1ULL << (best[info.idx].idx % half_n);
  used_idx ^= 1ULL << (info.nxt_idx % half_n);
  best[info.idx].idx = info.nxt_idx;
  best[info.idx].pos = info.pos;
  best[info.idx].st = info.st;
  best[info.idx].len = info.len;
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