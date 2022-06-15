#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <time.h>
#ifndef RELEASE
#include "audio_array_temp.hpp"
#else
#include "audio_array.hpp"
#endif
#define rep(i, n) for(int i = 0; i < (n); i++)
using ll = long long;
using uint = unsigned int;
using std::vector;
using std::max; using std::min;
using std::cout; using std::cin;
using std::cerr;

constexpr int inf = (unsigned int)-1 >> 1;
constexpr ll infl = (unsigned long long)-1 >> 1;

constexpr int n = 44*2; //candidate arrays
constexpr int half_n = n / 2;
constexpr int m = 20; //select num
constexpr int hz = 12000; //sampling hz[48k->12k]
constexpr int tot_frame = 98651; //max size of arrays[i]
constexpr int ans_length = hz * 8;
static_assert(m <= half_n);

// 数列の値の型
using Val_Type = int;

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

inline uint randxor32() noexcept{
  static uint x = rand() | rand() << 16;
  x ^= x << 13; x ^= x >> 17;
  return x ^= x << 5;
}
inline uint randxor128() noexcept{
  static uint x = rand() | rand() << 16;
  static uint y = rand() | rand() << 16;
  static uint z = rand() | rand() << 16;
  static uint w = rand() | rand() << 16;
  //static uint x=123456789,y=362436069,z=521288629,w=88675123;
  uint t=(x^(x<<11));x=y;y=z;z=w; return (w=(w^(w>>19))^(t^(t>>8)));
}
// returns random [l, r)
inline int rnd(const int &l, const int &r) noexcept{
  return randxor128() % (r - l) + l;
}

#define Weight(x) abs(x)
//#define Weight(x) ((ll)x*x)

// 問題の数列から数字を引いたやつのスコアを計算する
constexpr ll calc_score(const Val_Type a[ans_length]) noexcept{
  ll score = 0;
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
  if(cin.eof()) has_answer = false;

  // output answer_idx
  if(has_answer) rep(i, m){
    if(answer[i].idx < half_n) cout << "J" << answer[i].idx+1;
    else cout << "E" << answer[i].idx-half_n+1;
    cout << "\n";
  }
  cout << "\n";
}

void output_result(const Data best[m]){
  rep(i, m){
    if(best[i].idx < half_n) cout << "J" << best[i].idx+1;
    else cout << "E" << best[i].idx-half_n+1;
    cout << " " << best[i].pos * 4 << "\n";
  }
  if(has_answer){
    cout << "\n";
    int diff_num = 0;
    rep(i, m){
      bool ok = false;
      rep(j, m){
          if(best[i].idx == answer[j].idx){
          ok = true; break;
        }
      }
      if(ok) continue;
      diff_num++;
      if(best[i].idx < half_n) cout << "J" << best[i].idx+1;
      else cout << "E" << best[i].idx-half_n+1;
      cout << " ";
    }
    cerr << "Diff: " << diff_num << "/" << m << "\n";
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
int used_idx[half_n] = {};
Val_Type best_sub[ans_length];

ll best_score = infl;

void init(){
  memcpy(best_sub, problem, sizeof(Val_Type) * ans_length);
  // 最初は適当に値を入れておく
  rep(i, m){
    best[i].idx = i;
    best[i].pos = 0;
    best[i].st = 0;
    best[i].len = min(problem_length, audio_length[best[i].idx]);
    used_idx[i] = 1;
    // best_subの計算
    rep(j, best[i].len){
      best_sub[j + best[i].pos] -= arrays[best[i].idx][j + best[i].st];
    }
  }
  best_score = calc_score(best_sub);
  
  cerr << "First Score: " << best_score << "\n";
}

inline void rnd_create(RndInfo &change) noexcept{
  const int t = rnd(0, 10) >= 2;
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(0, m);
    change.nxt_idx = best[change.idx].idx;
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(0, audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(0, problem_length - change.len + 1);
  }
  // select other wav and swap and change pos
  else{
    change.idx = rnd(0, m);
    change.nxt_idx = rnd(0, n);
    while(used_idx[change.nxt_idx % half_n] && best[change.idx].idx % half_n != change.nxt_idx % half_n){
      change.nxt_idx = rnd(0, n);
    }
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(0, audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(0, problem_length - change.len + 1);
  }
}
inline RndInfo rnd_create() noexcept{
  RndInfo change;
  rnd_create(change);
  return change;
}

inline constexpr void calc_range_score_sub(const Val_Type a[], const Val_Type b[], const int &range, ll &score) noexcept{
  rep(i, range) score += Weight(b[i] - a[i]) - Weight(b[i]);
}
inline constexpr void calc_range_score_add(const Val_Type a[], const Val_Type b[], const int &range, ll &score) noexcept{
  rep(i, range) score += Weight(b[i] + a[i]) - Weight(b[i]);
}

inline constexpr ll calc_one_changed_ans(const RndInfo &info) noexcept{
  const Data &pre = best[info.idx];
  const int info_rig = info.pos + info.len;
  const int pre_rig = pre.pos + pre.len;
  const Val_Type *arr_info = arrays[info.nxt_idx] + info.st;
  const Val_Type *arr_pre = arrays[pre.idx] + pre.st;
  ll score = best_score;
  // cross
  if(max(info.pos, pre.pos) < min(info_rig, pre_rig)){
    // 左側がleft
    if(info.pos <= pre.pos){
      const int rightest = min(info_rig, pre.pos);
      calc_range_score_sub(arr_info, best_sub+info.pos, rightest-info.pos, score);
    }
    // 左側がright
    else{
      const int rightest = min(pre_rig, info.pos);
      calc_range_score_add(arr_pre, best_sub+pre.pos, rightest-pre.pos, score);
    }
    // 右側がright
    if(info_rig <= pre_rig){
      const int leftest = max(pre.pos, info_rig);
      const int s = max(info_rig - pre.pos, 0);
      calc_range_score_add(arr_pre+s, best_sub+leftest, pre_rig-leftest, score);
    }
    // 右側がleft
    else{
      const int leftest = max(info.pos, pre_rig);
      const int s = max(pre_rig - info.pos, 0);
      calc_range_score_sub(arr_info+s, best_sub+leftest, info_rig-leftest, score);
    }
    // middle
    const int leftest = max(info.pos, pre.pos);
    const int range = min(info_rig, pre_rig) - leftest;
    const int sl = max(pre.pos - info.pos, 0);
    const int sr = max(info.pos - pre.pos, 0);
    rep(i, range){
      score -= Weight(best_sub[i+leftest]);
      const Val_Type d = best_sub[i+leftest] - arr_info[i+sl] + arr_pre[i+sr];
      score += Weight(d);
    }
  }
  // not cross
  else{
    calc_range_score_sub(arr_info, best_sub+info.pos, info.len, score);
    calc_range_score_add(arr_pre, best_sub+pre.pos, pre.len, score);
  }
  return score;
}


void update_values(const RndInfo &info){
  // update best_sub
  const Data &pre = best[info.idx];
  rep(i, pre.len){
    best_sub[i + pre.pos] += arrays[pre.idx][i + pre.st];
  }
  rep(i, info.len){
    best_sub[i + info.pos] -= arrays[info.nxt_idx][i + info.st];
  }
  // update info
  used_idx[best[info.idx].idx % half_n]--;
  used_idx[info.nxt_idx % half_n]++;
  best[info.idx].idx = info.nxt_idx;
  best[info.idx].pos = info.pos;
  best[info.idx].st = info.st;
  best[info.idx].len = info.len;
}

}; // namespace solver


struct ios_do_not_sync {
  ios_do_not_sync(){
    std::cin.tie(nullptr);
    std::ios::sync_with_stdio(false);
  }
} ios_do_not_sync_instance;