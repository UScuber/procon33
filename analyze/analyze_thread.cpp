#include <thread>
#include <future>
#include "library.hpp"
using namespace std;

constexpr double limit_time = 40.0;

#define isTestCase

vector<vector<Val_Type>> arrays[n];
vector<vector<Val_Type>> problem(ans_length, vector<Val_Type>(dhz));
int answer_idx[m];
int answer_pos[m];

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
  TestCase::make_random(arrays, problem, answer_idx, answer_pos);
  // output answer_idx
  rep(i, m){
    cout << answer_idx[i] << " " << answer_pos[i] << "\n";
  }
  cout << "\n";
}
void read(){
  File::read_values(arrays, problem, answer_idx, answer_pos, cin);
  // output answer_idx, pos
  rep(i, m){
    cout << answer_idx[i] << " " << answer_pos[i] << "\n";
  }
  cout << "\n";
}


namespace solver {

struct RndInfo {
  int idx,pos, nxt_idx;
};

constexpr int thread_num = 12;
constexpr int tasks_num = 50;

int best_pos[m] = {};
int best_select_idx[m];
int used_idx[n] = {};


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
      if(spend_time > limit_time*2/5) break;
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
  double temp_time = spend_time;
  int cnt = 0;
  for(; ; steps += thread_num * tasks_num){
    {
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time) break;
    }
    cnt++;
    future<pair<ll, RndInfo>> threads[thread_num];
    RndInfo rnd_arrays[thread_num][tasks_num];
    rep(i, thread_num){
      rep(j, tasks_num){
        rnd_arrays[i][j] = rnd_create();
      }
      threads[i] = async(solve_one_thread, rnd_arrays[i]);
    }
    RndInfo best_change{ -1,-1,-1 };
    ll good_score = infl;
    rep(i, thread_num){
      ll score; RndInfo res;
      tie(score, res) = threads[i].get();
      if(good_score > score){
        good_score = score;
        best_change = res;
      }
    }
    
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
  cerr << "Time per loop: " << (spend_time-temp_time)/cnt << "\n";
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
  cin.tie(nullptr);
  ios::sync_with_stdio(false);
  #ifdef isTestCase
    read();
  #else
    init();
  #endif
  solver::solve();
}