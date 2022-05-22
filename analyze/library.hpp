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

constexpr int n = 44*2; //candidate arrays
constexpr int m = 20; //select num
constexpr int fps = 30;
constexpr int tot_time = 10; //[s]
constexpr int tot_frame = fps * tot_time;
constexpr int dhz = 600;
constexpr int ans_length = tot_frame * 2;
static_assert(m <= n);
static_assert(fps <= tot_frame);
constexpr double PI = 3.141592653589793238;

// 数列の値の型
using Val_Type = int;

Val_Type arrays[n][tot_frame][dhz];
Val_Type problem[ans_length][dhz] = {};

struct Data {
  int idx; //札の種類
  int pos; //貼り付け位置
  int st; //札の再生開始位置
  int len; //札の再生の長さ
};
Data answer[m];


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
void make_rnd_array(Val_Type v[dhz]){
  int first_hz = rnd(6, 18);
  rep(i, dhz){
    double arc = (1 + cos((double)i / first_hz)) * 4.5;
    arc *= arc;
    v[i] = arc * rnd(7, 14)/10.0;
    if(i % 50 == 0) first_hz++;
  }
}

// 1次元配列の加算・減算
inline void add(Val_Type a[dhz], const Val_Type b[dhz]) noexcept{
  rep(i, dhz) a[i] += b[i];
}
inline void sub(Val_Type a[dhz], const Val_Type b[dhz]) noexcept{
  rep(i, dhz) a[i] -= b[i];
}

// 問題の数列から数字を引いたやつのスコアを計算する
ll calc_score(const Val_Type a[ans_length][dhz]) noexcept{
  ll score = 0;
  rep(i, ans_length){
    Val_Type tot = 0;
    rep(j, dhz){
      //tot += a[i][j] * a[i][j];
      tot += abs(a[i][j]);
    }
    score += tot;
  }
  return score;
}


namespace File {

void read_values(std::istream &is){
  rep(i, n){
    rep(j, tot_frame){
      rep(k, dhz) is >> arrays[i][j][k];
    }
  }
  rep(i, ans_length){
    rep(j, dhz) is >> problem[i][j];
  }
  rep(i, m) is >> answer[i].idx;
  rep(i, m) is >> answer[i].pos;
  rep(i, m) is >> answer[i].st;
  rep(i, m) is >> answer[i].len;
}

}; // namespace File


namespace solver {

struct RndInfo {
  int idx; //変更する値
  int pos; //変更後の貼り付け位置
  int nxt_idx; //新しく更新する札
  int st; //次の札の再生開始位置
  int len; //次の札の再生の長さ
};

Data best[m];
int used_idx[n] = {};
Val_Type best_sub[ans_length][dhz];
Val_Type temp_arr[ans_length][dhz];

ll best_score = infl;


inline RndInfo rnd_create() noexcept{
  const int t = rnd(0, 10) >= 2;
  RndInfo change;
  // select wav and change pos
  if(t == 0){
    change.idx = rnd(0, m);
    change.nxt_idx = best[change.idx].idx;
    change.pos = rnd(0, tot_frame);
    change.st = rnd(0, tot_frame - fps);
    change.len = rnd(fps, tot_frame - change.st + 1);
  }
  // select other wav and swap and change pos
  else{
    change.idx = rnd(0, m);
    change.nxt_idx = rnd(0, n);
    while(used_idx[change.nxt_idx]) change.nxt_idx = rnd(0, n);
    change.pos = rnd(0, tot_frame);
    change.st = rnd(0, tot_frame - fps);
    change.len = rnd(fps, tot_frame - change.st + 1);
  }
  return change;
}
// flip==-1 -> (left,right)=(info,pre), flip==-1 -> (left,right)=(pre,info)
ll calc_one_changed_ans2(const RndInfo &left, const RndInfo &right, const int &flip = 1) noexcept{
  assert(left.pos <= right.pos);
  ll score = best_score;
  // cross
  if(max(left.pos, right.pos) < min(left.pos+left.len, right.pos+right.len)){
    if(left.pos <= right.pos){

    }
    if(left.pos + left.len <= right.pos + right.len){
      
    }
  }
  // not cross
  else{
    rep(i, left.len){
      rep(j, dhz){
        score -= abs(best_sub[i + left.pos][j]);
        const Val_Type d = best_sub[i + left.pos][j] - flip*arrays[left.nxt_idx][i + left.st][j];
        score += abs(d);
      }
    }
    rep(i, right.len){
      rep(j, dhz){
        score -= abs(best_sub[i + right.pos][j]);
        const Val_Type d = best_sub[i + right.pos][j] + flip*arrays[right.nxt_idx][i + right.st][j];
        score += abs(d);
      }
    }
  }
}

ll calc_one_changed_ans(const RndInfo &info) noexcept{
  const Data &pre = best[info.idx];
  rep(i, pre.len){
    add(best_sub[i + pre.pos], arrays[pre.idx][i + pre.st]);
  }
  rep(i, info.len){
    sub(best_sub[i + info.pos], arrays[info.nxt_idx][i + info.st]);
  }
  const ll score = calc_score(best_sub);
  rep(i, pre.len){
    sub(best_sub[i + pre.pos], arrays[pre.idx][i + pre.st]);
  }
  rep(i, info.len){
    add(best_sub[i + info.pos], arrays[info.nxt_idx][i + info.st]);
  }
  return score;
}


void update_values(const RndInfo &info){
  // update best_sub
  const Data &pre = best[info.idx];
  rep(i, pre.len){
    add(best_sub[i + pre.pos], arrays[pre.idx][i + pre.st]);
  }
  rep(i, info.len){
    sub(best_sub[i + info.pos], arrays[info.nxt_idx][i + info.st]);
  }
  // update info
  used_idx[best[info.idx].idx]--;
  used_idx[info.nxt_idx]++;
  best[info.idx].idx = info.nxt_idx;
  best[info.idx].pos = info.pos;
  best[info.idx].st = info.st;
  best[info.idx].len = info.len;
}

}; // namespace solver