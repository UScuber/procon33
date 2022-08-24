#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <time.h>
#include <cstring>
#include <chrono>
//#pragma GCC target("avx2")
#pragma GCC optimize("unroll-loops")
#ifndef RELEASE
#include "audio_array_temp.hpp"
#else
#include "audio/wave.hpp"
#include "audio_array.hpp"
#endif
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
  return (x ^= x << 5) % (r - l) + l;
}
// returns random [0, rng)
inline int rnd(const int rng) noexcept{
  static uint x = rand() | rand() << 16;
  x ^= x << 13; x ^= x >> 17;
  return (x ^= x << 5) % rng;
}

namespace {
  constexpr double b1table[32] = {
    0x1.0000000000000p+0,
    0x1.059b0d3158574p+0,
    0x1.0b5586cf9890fp+0,
    0x1.11301d0125b51p+0,
    0x1.172b83c7d517bp+0,
    0x1.1d4873168b9aap+0,
    0x1.2387a6e756238p+0,
    0x1.29e9df51fdee1p+0,
    0x1.306fe0a31b715p+0,
    0x1.371a7373aa9cbp+0,
    0x1.3dea64c123422p+0,
    0x1.44e086061892dp+0,
    0x1.4bfdad5362a27p+0,
    0x1.5342b569d4f82p+0,
    0x1.5ab07dd485429p+0,
    0x1.6247eb03a5585p+0,
    0x1.6a09e667f3bcdp+0,
    0x1.71f75e8ec5f74p+0,
    0x1.7a11473eb0187p+0,
    0x1.82589994cce13p+0,
    0x1.8ace5422aa0dbp+0,
    0x1.93737b0cdc5e5p+0,
    0x1.9c49182a3f090p+0,
    0x1.a5503b23e255dp+0,
    0x1.ae89f995ad3adp+0,
    0x1.b7f76f2fb5e47p+0,
    0x1.c199bdd85529cp+0,
    0x1.cb720dcef9069p+0,
    0x1.d5818dcfba487p+0,
    0x1.dfc97337b9b5fp+0,
    0x1.ea4afa2a490dap+0,
    0x1.f50765b6e4540p+0,
  };
  constexpr double b2table[32] = {
    0x1.0000000000000p+0,
    0x1.002c605e2e8cfp+0,
    0x1.0058c86da1c0ap+0,
    0x1.0085382faef83p+0,
    0x1.00b1afa5abcbfp+0,
    0x1.00de2ed0ee0f5p+0,
    0x1.010ab5b2cbd11p+0,
    0x1.0137444c9b5b5p+0,
    0x1.0163da9fb3335p+0,
    0x1.019078ad6a19fp+0,
    0x1.01bd1e77170b4p+0,
    0x1.01e9cbfe113efp+0,
    0x1.02168143b0281p+0,
    0x1.02433e494b755p+0,
    0x1.027003103b10ep+0,
    0x1.029ccf99d720ap+0,
    0x1.02c9a3e778061p+0,
    0x1.02f67ffa765e6p+0,
    0x1.032363d42b027p+0,
    0x1.03504f75ef071p+0,
    0x1.037d42e11bbccp+0,
    0x1.03aa3e170aafep+0,
    0x1.03d7411915a8ap+0,
    0x1.04044be896ab6p+0,
    0x1.04315e86e7f85p+0,
    0x1.045e78f5640b9p+0,
    0x1.048b9b35659d8p+0,
    0x1.04b8c54847a28p+0,
    0x1.04e5f72f654b1p+0,
    0x1.051330ec1a03fp+0,
    0x1.0540727fc1762p+0,
    0x1.056dbbebb786bp+0,
  };
  constexpr inline uint64_t exp_table(const uint64_t s) noexcept{
    const double b = b1table[s>>5&31] * b2table[s&31];
    return *(uint64_t*)&b + ((s >> 10) << 52);
  }
}

constexpr double inline fast_exp(const double x) noexcept{
  if(x < -104.0f) return 0.0;
  if(x > 0x1.62e42ep+6f) return HUGE_VALF;
  constexpr double R = 0x3.p+51f;
  constexpr double iln2 = 0x1.71547652b82fep+10;
  constexpr double ln2h = 0x1.62e42fefc0000p-11;
  constexpr double ln2l = -0x1.c610ca86c3899p-47;
  const double k_R = x*iln2+R;
  const double t = x*iln2*(-ln2l+ln2h+x);
  const uint64_t exp_s = exp_table(*(uint64_t*)&k_R);
  return *(double*)&exp_s * ((1.0/6.0*t+1.0/2.0)*t*t+t+1);
}

#define Weight(x) abs(x)
// check is crossed [a, b), [c, d)
#define is_cross(a,b,c,d) (max(a,c) < min(b,d))

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
  // output audio
  /*
  constexpr int p = default_sampling_hz / analyze_sampling_hz;
  vector<Wave> waves(m);
  char buf[64];
  rep(i, m){
    sprintf(buf, "audio/JKspeech/%c%02d.wav", best[i].idx >= half_n ? 'E' : 'J', best[i].idx % half_n + 1);
    Wave tmp;
    read_audio(tmp, buf);
    //assersion found
    const auto res = separate_audio(tmp, { 0, best[i].st*p, (best[i].st+best[i].len)*p, tmp.L });
    waves[i] = res[1];
  }
  vector<int> st(m);
  rep(i, m) st[i] = best[i].pos*p;
  rep(i, m){
    vector<Wave> wav;
    vector<int> st_pos;
    rep(j, m) if(i != j){
      wav.emplace_back(waves[j]);
      st_pos.emplace_back(best[i].pos*p);
    }
    const auto result = merge_audio(wav, st_pos);
    cerr << best[i].idx << " ";
    sprintf(buf, "outs/out%d.wav", i+1);
    write_audio(result, buf);
  }
  */
  cerr << "\n";

  if(has_answer){
    /*
    cout << "\n";
    */
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
    rep(i, m){
      cout << "Fuda: " << best[i].idx << ", Pos: " << best[i].pos << ", St: " << best[i].st << ", Len: " << best[i].len << "\n";
    }
    cerr << "Audio Diff: " << audio_diff_num << "/" << m << "\n";
    cerr << "Karuta Diff: " << karuta_diff_num << "/" << m << "\n";
    cout << audio_diff_num << " " << karuta_diff_num << "\n";
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
  const int t = rnd(6);
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(problem_length - change.len + 1);
  }
  // select other wav and swap and change pos
  else if(t == 1){
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
  // select other wav and swap and change pos
  /*
  change.idx = rnd(m);
  change.nxt_idx = rnd(n);
  while((used_idx >> (change.nxt_idx % half_n) & 1) && best[change.idx].idx % half_n != change.nxt_idx % half_n){
    change.nxt_idx = rnd(n);
  }
  change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
  change.st = rnd(audio_length[change.nxt_idx] - change.len + 1);
  change.pos = rnd(problem_length - change.len + 1);
  */
}
inline void rnd_create2(RndInfo &change) noexcept{
  const int t = rnd(3);
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(m);
    change.nxt_idx = best[change.idx].idx;
    change.len = rnd(hz, min(problem_length, audio_length[change.nxt_idx]) + 1);
    change.st = rnd(audio_length[change.nxt_idx] - change.len + 1);
    change.pos = rnd(problem_length - change.len + 1);
  }
  // select other wav and swap and change pos
  else if(t == 1){
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

inline constexpr void calc_range_score_sub(const Val_Type a[], const Val_Type b[], const int range, Score_Type &score) noexcept{
  rep(i, range) score += Weight(b[i] - a[i]) - Weight(b[i]);
}
inline constexpr void calc_range_score_add(const Val_Type a[], const Val_Type b[], const int range, Score_Type &score) noexcept{
  rep(i, range) score += Weight(b[i] + a[i]) - Weight(b[i]);
}

inline constexpr Score_Type calc_one_changed_ans(const RndInfo &info) noexcept{
  const Data &pre = best[info.idx];
  const int info_rig = info.pos + info.len;
  const int pre_rig = pre.pos + pre.len;
  const Val_Type *arr_info = arrays[info.nxt_idx] + info.st;
  const Val_Type *arr_pre = arrays[pre.idx] + pre.st;
  Score_Type score = best_score;
  // cross
  if(is_cross(info.pos, info_rig, pre.pos, pre_rig)){
  //if(max(info.pos, pre.pos) < min(info_rig, pre_rig)){
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
      score += Weight(best_sub[i+leftest] - arr_info[i+sl] + arr_pre[i+sr]);
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