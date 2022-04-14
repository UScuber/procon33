#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <time.h>
#define rep(i, n) for(int i = 0; i < (n); i++)
using ll = long long;
using uint = unsigned int;
using std::vector;

constexpr int inf = std::numeric_limits<int>::max();
constexpr ll infl = std::numeric_limits<ll>::max();

constexpr int n = 44; //candidate arrays
constexpr int m = 20; //select num
constexpr int fps = 30;
constexpr int tot_time = 10;
constexpr int tot_frame = fps * tot_time;
constexpr int dhz = 600;
constexpr int ans_length = tot_frame * 2;
static_assert(m <= n);
const double PI = acos(-1);

// 数列の値の型
using Val_Type = int;

vector<vector<Val_Type>> arrays[n];


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

// 波のあるランダムな値を生成
vector<Val_Type> make_rnd_array(int n){
  const int first_hz = rnd(5, 10);
  vector<Val_Type> res(n);
  rep(i, n){
    double arc = cos(PI * i / first_hz) * 30;
    arc *= arc;
    res[i] = arc * rnd(7, 14)/10.0;
  }
  return res;
}

// 1次元配列の加算・減算
inline void add(vector<Val_Type> &a, const vector<Val_Type> &b) noexcept{
  rep(i, b.size()) a[i] += b[i];
}
inline void sub(vector<Val_Type> &a, const vector<Val_Type> &b) noexcept{
  rep(i, b.size()) a[i] -= b[i];
}

// 問題の数列から数字を引いたやつのスコアを計算する
ll calc_score(const vector<vector<Val_Type>> &a) noexcept{
  const int len = a[0].size();
  Val_Type score = 0;
  rep(i, a.size()){
    Val_Type tot = 0;
    rep(j, len){
      //tot += a[i][j] * a[i][j];
      tot += abs(a[i][j]);
    }
    score += tot;
  }
  return score;
}


namespace File {

void read_values(vector<vector<Val_Type>> arrays[n], vector<vector<Val_Type>> &problem,
                int answer_idx[m], int answer_pos[m], std::istream &is){
  assert(problem.size() == ans_length);
  rep(i, n){
    arrays[i].assign(tot_frame, vector<Val_Type>(dhz));
    rep(j, tot_frame){
      rep(k, dhz) is >> arrays[i][j][k];
    }
  }
  rep(i, ans_length){
    rep(j, dhz) is >> problem[i][j];
  }
  rep(i, m) is >> answer_idx[i];
  rep(i, m) is >> answer_pos[i];
}

}; // namespace File

namespace TestCase {

void make_random(vector<vector<Val_Type>> arrays[n], vector<vector<Val_Type>> &problem,
                int answer_idx[m], int answer_pos[m]){
  assert(problem.size() == ans_length);
  rep(i, n){
    arrays[i].resize(tot_frame);
    rep(j, tot_frame){
      arrays[i][j] = make_rnd_array(dhz);
    }
  }
  // make answer_idx
  int used[n] = {};
  rep(i, m){
    const int ran = rnd(0, n);
    if(used[ran]){
      i--; continue;
    }
    answer_idx[i] = ran;
    used[ran] = 1;
  }
  rep(i, m){
    assert(0 <= answer_idx[i] && answer_idx[i] < n);
    const int pos = rnd(0, tot_frame);
    rep(j, tot_frame){
      add(problem[j + pos], arrays[answer_idx[i]][j]);
    }
    answer_pos[i] = pos;
  }
  // ランダムに値をずらす
  rep(i, ans_length){
    rep(j, dhz){
      const int t = rnd(0, 2);
      if(t) continue;
      problem[i][j] = rnd(7,14)/10.0 * problem[i][j];
    }
  }
}

}; // namespace TestCase

namespace solver {

struct RndInfo {
  int idx,pos, nxt_idx;
};

int best_pos[m] = {};
int best_select_idx[m];
int used_idx[n] = {};
vector<vector<Val_Type>> best_sub(ans_length, vector<Val_Type>(dhz));

inline RndInfo rnd_create(){
  const int t = rnd(0, 10) >= 2;
  RndInfo change{ -1,-1,-1 };
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(0, m);
    change.nxt_idx = best_select_idx[change.idx];
    change.pos = rnd(0, tot_frame);
  }
  // select other wav and swap and change pos
  else{
    change.idx = rnd(0, m);
    change.nxt_idx = rnd(0, n);
    while(used_idx[change.nxt_idx]) change.nxt_idx = rnd(0, n);
    change.pos = rnd(0, tot_frame);
  }
  return change;
}

ll calc_one_changed_ans(const RndInfo &info){
  auto temp = best_sub;
  const int pre_pos = best_pos[info.idx];
  const int pre_idx = best_select_idx[info.idx];
  rep(i, tot_frame){
    add(temp[i + pre_pos], arrays[pre_idx][i]);
  }
  rep(i, tot_frame){
    sub(temp[i + info.pos], arrays[info.nxt_idx][i]);
  }
  return calc_score(temp);
}


void update_values(const RndInfo &info){
  const int pre_pos = best_pos[info.idx];
  const int pre_idx = best_select_idx[info.idx];
  // update best_sub
  rep(i, tot_frame){
    add(best_sub[i + pre_pos], arrays[pre_idx][i]);
  }
  rep(i, tot_frame){
    sub(best_sub[i + info.pos], arrays[info.nxt_idx][i]);
  }
  // update info
  used_idx[best_select_idx[info.idx]]--;
  used_idx[info.nxt_idx]++;
  best_select_idx[info.idx] = info.nxt_idx;
  best_pos[info.idx] = info.pos;
}

}; // namespace solver