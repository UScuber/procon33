#include <bits/stdc++.h>
#include <thread>
#include <future>
#define rep(i, n) for(int i = 0; i < (n); i++)
using ll = long long;
using uint = unsigned int;
using namespace std;

constexpr int inf = numeric_limits<int>::max();
constexpr ll infl = numeric_limits<ll>::max();

constexpr int n = 44; //candidate arrays
constexpr int m = 20; //select num
constexpr int fps = 30;
constexpr int tot_time = 5;
constexpr int tot_frame = fps * tot_time;
constexpr int dhz = 600;
constexpr int ans_length = tot_frame * 2;

constexpr double limit_time = 40.0;

using Val_Type = int;

vector<vector<Val_Type>> arrays[n];
vector<vector<Val_Type>> problem;
int answer_idx[m];
int answer_pos[m];

inline uint randxor(){
  static uint x = rand() | rand() << 16;
  static uint y = rand() | rand() << 16;
  static uint z = rand() | rand() << 16;
  static uint w = rand() | rand() << 16;
  //static uint x=123456789,y=362436069,z=521288629,w=88675123;
  uint t;
  t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}
// returns random [l, r)
inline int rnd(const int &l, const int &r){
  return randxor() % (r - l) + l;
}
const double PI = acos(-1);
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
inline void add(vector<Val_Type> &a, const vector<Val_Type> &b){
  assert(b.size() <= a.size());
  rep(i, b.size()) a[i] += b[i];
}
inline void sub(vector<Val_Type> &a, const vector<Val_Type> &b){
  assert(b.size() <= a.size());
  rep(i, b.size()) a[i] -= b[i];
}

// problemから数字を引いたやつのスコアを計算する
ll calc_score(const vector<vector<Val_Type>> &a){
  Val_Type score = 0;
  rep(i, a.size()){
    Val_Type tot = 0;
    rep(j, a[0].size()){
      //tot += a[i][j] * a[i][j];
      tot += abs(a[i][j]);
    }
    score += tot;
  }
  return score;
}

// 答えと仮定したやつを引いて、計算する
ll calc_selected_ans(const int ans[m], const int pos[m]){
  auto temp = problem;
  rep(i, m){
    rep(j, tot_frame){
      sub(temp[j + pos[i]], arrays[ans[i]][j]);
    }
  }
  return calc_score(temp);
}

void init(){
  static_assert(m <= n);
  rep(i, n){
    arrays[i].resize(tot_frame);
    rep(j, tot_frame){
      arrays[i][j] = make_rnd_array(dhz);
    }
  }
  // make answer_idx
  int used[n] = {};
  rep(i, m){
    int ran = rnd(0, n);
    if(used[ran]){
      i--; continue;
    }
    answer_idx[i] = ran;
    used[ran] = 1;
  }
  problem.assign(ans_length, vector<Val_Type>(dhz));
  rep(i, m){
    assert(0 <= answer_idx[i] && answer_idx[i] < n);
    const int pos = rnd(0, tot_frame);
    rep(j, tot_frame){
      add(problem[j + pos], arrays[answer_idx[i]][j]);
    }
    answer_pos[i] = pos;

    // output answer_idx
    cout << answer_idx[i] << " " << pos << "\n";
  }
  cout << "\n";
}

namespace solver {

struct RndInfo {
  int idx,pos, nxt_idx;
};

constexpr int thread_num = 10;
constexpr int tasks_num = 2;

int best_pos[m] = {};
int best_select_idx[m];
int used_idx[n] = {};

RndInfo random_array[thread_num][m];

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

void create_rndinfo(RndInfo ran[tasks_num]){
  rep(i, tasks_num){
    ran[i] = rnd_create();
  }
}

// 一番いいデータbest_*から推測
ll calc_one_changed_ans(const RndInfo &info){
  auto temp = problem;
  rep(i, m) if(info.idx != i){
    rep(j, tot_frame){
      sub(temp[j + best_pos[i]], arrays[best_select_idx[i]][j]);
    }
  }
  rep(i, tot_frame){
    sub(temp[i + info.pos], arrays[info.nxt_idx][i]);
  }
  return calc_score(temp);
}

pair<ll, RndInfo> solve_one_thread(const RndInfo ran[tasks_num]){
  ll best_score = infl;
  RndInfo best_change{ -1,-1,-1 };
  rep(i, tasks_num){
    const ll score = calc_one_changed_ans(ran[i]);
    if(best_score > score){
      best_score = score;
      best_change = ran[i];
    }
  }
  assert(best_change.idx != -1);
  return { best_score, best_change };
}

void solve(){
  // 適当にはじめは値を入れておく
  rep(i, m){
    best_select_idx[i] = i;
    used_idx[i] = 1;
  }
  ll best_score = calc_selected_ans(best_select_idx, best_pos);
  
  cerr << "First Score: " << best_score << "\n";

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  double spend_time = 0;
  const clock_t start_time = clock();
  // 山登り法(single thread)
  cerr << "Start Single Thread\n";
  for(; ; steps++){
    constexpr int mask = (1 << 5) - 1;
    if(!(steps & mask)){
      const clock_t end_time = clock();
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time*2/3) break;
    }
    RndInfo change = rnd_create();
    const ll score = calc_one_changed_ans(change);
    if(best_score > score){
      best_score = score;
      // 値の更新
      used_idx[best_select_idx[change.idx]]--;
      used_idx[change.nxt_idx]++;
      best_select_idx[change.idx] = change.nxt_idx;
      best_pos[change.idx] = change.pos;
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }
  }
  cerr << "\n";
  cerr << "Start Multi Thread\n";
  // 山登り法(multi thread)
  for(; ; steps += thread_num * tasks_num){
    {
      const clock_t end_time = clock();
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time) break;
    }
    future<pair<ll, RndInfo>> threads[thread_num];
    RndInfo rnd_arrays[thread_num][tasks_num];
    rep(i, thread_num){
      create_rndinfo(rnd_arrays[i]);
      threads[i] = async(solve_one_thread, rnd_arrays[i]);
    }
    RndInfo best_change{ -1,-1,-1 };
    ll good_score = infl;
    rep(i, thread_num){
      ll score = infl;
      RndInfo res;
      tie(score, res) = threads[i].get();
      if(good_score > score){
        good_score = score;
        best_change = res;
      }
    }
    assert(best_change.idx != -1);
    // 更新できなかった時に復元する
    //const ll score = calc_selected_ans(best_select_idx, best_pos);
    if(good_score < best_score){
      best_score = good_score;
      // 値の更新
      used_idx[best_select_idx[best_change.idx]]--;
      used_idx[best_change.nxt_idx]++;
      best_select_idx[best_change.idx] = best_change.nxt_idx;
      best_pos[best_change.idx] = best_change.pos;
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }
  }
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Final Score: " << best_score << "\n";

  // output result
  rep(i, m){
    cout << best_select_idx[i] << " " << best_pos[i] << "\n";
  }
  cout << "\n";
  int diff_num = 0;
  rep(i, m){
    bool ok = false;
    rep(j, m) if(best_select_idx[i] == answer_idx[j] && best_pos[i] == answer_pos[j]){
      ok = true; break;
    }
    if(ok) continue;
    diff_num++;
    cout << best_select_idx[i] << " " << best_pos[i] << "\n";
  }
  cerr << "Diff: " << diff_num << "/" << m << "\n";
}

}; // namespace solver


int main(){
  srand(time(NULL));
  init();
  // ランダムに値をずらす
  rep(i, ans_length){
    rep(j, dhz){
      const int t = rnd(0, 2);
      if(t) continue;
      problem[i][j] = rnd(7,14)/10.0 * problem[i][j];
    }
  }
  solver::solve();
}