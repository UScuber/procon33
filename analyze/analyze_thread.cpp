#include <thread>
#include <future>
#include "library.hpp"
using std::pair;

constexpr double limit_time = 60.0 * 5;


void read(){
  File::read_values(cin);
  // output answer_idx
  if(has_answer) rep(i, m){
    cout << answer[i].idx << "\n";
    if(answer[i].idx < n/2) cout << "J" << answer[i].idx+1;
    else cout << "E" << answer[i].idx-44+1;
    cout << "\n";
  }
  cout << "\n";
}


namespace solver {

constexpr int thread_num = 12;
constexpr int tasks_num = 512 * 8;

RndInfo rnd_arrays[thread_num][tasks_num];


pair<ll, RndInfo> solve_one_thread(const RndInfo ran[tasks_num]){
  ll best_sc = infl;
  RndInfo best_change;
  rep(i, tasks_num){
    const ll score = calc_one_changed_ans(ran[i]);
    if(best_sc > score){
      best_sc = score;
      best_change = ran[i];
    }
  }
  return { best_sc, best_change };
}

void solve(){
  init();

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
    std::future<pair<ll, RndInfo>> threads[thread_num];
    rep(i, thread_num){
      rep(j, tasks_num){
        rnd_create(rnd_arrays[i][j]);
      }
      threads[i] = std::async(solve_one_thread, rnd_arrays[i]);
    }
    RndInfo best_change;
    ll good_score = infl;
    rep(i, thread_num){
      ll score; RndInfo res;
      std::tie(score, res) = threads[i].get();
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
    if(best[i].idx < n/2) cout << "J" << best[i].idx+1;
    else cout << "E" << best[i].idx-44+1;
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
      if(best[i].idx < n/2) cout << "J" << best[i].idx+1;
      else cout << "E" << best[i].idx-44+1;
      cout << " ";
    }
    cerr << "Diff: " << diff_num << "/" << m << "\n";
  }
}

}; // namespace solver


int main(){
  srand(time(NULL));

  read();

  solver::solve();
}