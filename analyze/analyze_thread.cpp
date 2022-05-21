#include <thread>
#include <future>
#include "library.hpp"
using namespace std;

constexpr double limit_time = 60.0 * 10;


void read(){
  File::read_values(cin);
  // output answer_idx, pos
  rep(i, m){
    cout << answer[i].idx << " " << answer[i].pos << "\n";
  }
  cout << "\n";
}


namespace solver {

constexpr int thread_num = 12;
constexpr int tasks_num = 512;


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
  best_sub = problem;
  // 最初は適当に値を入れておく
  rep(i, m){
    best[i].idx = i;
    best[i].pos = 0;
    best[i].st = 0;
    best[i].len = tot_frame;
    used_idx[i] = 1;
    // best_subの計算
    rep(j, best[i].len){
      sub(best_sub[j + best[i].pos], arrays[i][j + best[i].st]);
    }
  }
  ll best_score = calc_score(best_sub);
  
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
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time*2/5) break;
    }
    const RndInfo change = rnd_create();
    const ll score = calc_one_changed_ans(change);
    if(best_score > score){
      best_score = score;
      update_values(change);
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
    cnt += thread_num * tasks_num;
    future<pair<ll, RndInfo>> threads[thread_num];
    RndInfo rnd_arrays[thread_num][tasks_num];
    rep(i, thread_num){
      rep(j, tasks_num){
        rnd_arrays[i][j] = rnd_create();
      }
      threads[i] = async(solve_one_thread, rnd_arrays[i]);
    }
    RndInfo best_change;
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
      update_values(best_change);
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
    cout << best[i].idx << " " << best[i].pos << "\n";
  }
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
    cout << best[i].idx << " " << best[i].pos << "\n";
  }
  cerr << "Diff: " << diff_num << "/" << m << "\n";
}

}; // namespace solver


int main(){
  srand(time(NULL));
  cin.tie(nullptr);
  ios::sync_with_stdio(false);

  read();

  solver::solve();
}