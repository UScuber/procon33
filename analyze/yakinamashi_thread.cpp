#include <thread>
#include <future>
#include "library.hpp"

constexpr double limit_time = 60.0 * 5;


namespace Solver {

constexpr int thread_num = 12 + 4;
constexpr int tasks_num = 512 * 8 / 4;

RndInfo rnd_arrays[thread_num][tasks_num];

Data awesome[m];
ll awesome_score = infl;

std::pair<ll, RndInfo> solve_one_thread(const RndInfo ran[tasks_num]){
  ll best_sc = infl;
  int best_change_idx = -1;
  rep(i, tasks_num){
    const ll score = calc_one_changed_ans(ran[i]);
    if(best_sc > score){
      best_sc = score;
      best_change_idx = i;
    }
  }
  return { best_sc, ran[best_change_idx] };
}

void solve(){
  init();
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  constexpr double t0 = 4e3;
  constexpr double t1 = 1e2;
  double temp = t0;
  double spend_time = 0;
  const clock_t start_time = clock();
  // 焼きなまし法(single thread)
  cerr << "Start Single Thread\n";
  for(; ; steps++){
    constexpr int mask = (1 << 7) - 1;
    if(!(steps & mask)){
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time*3/5) break;
      //temp = pow(t0, 1.0-spend_time/(limit_time*3/5)) * pow(t1, spend_time/(limit_time*3/5));
      temp = pow(t0, 1.0-spend_time/(limit_time)) * pow(t1, spend_time/(limit_time));
    }
    const RndInfo change = rnd_create();
    const ll score = calc_one_changed_ans(change);
    if(awesome_score > score){
      awesome_score = score;
      best_score = score;
      update_values(change);
      memcpy(awesome, best, sizeof(awesome));
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }else if(exp((double)(best_score - score) / temp) > rnd(0,1024)/1024.0){
      best_score = score;
      update_values(change);
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }
  }
  best_score = awesome_score;
  memcpy(best, awesome, sizeof(best));
  rep(i, half_n) used_idx[i] = 0;
  rep(i, m) used_idx[best[i].idx]++;
  cerr << "\n";
  cerr << "Score: " << best_score << "\n";
  cerr << "Start Multi Thread\n";
  // 山登り法(multi thread)
  double temp_time = spend_time;
  int cnt = 0;
  for(; ; steps += thread_num * tasks_num){
    {
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time) break;
      temp = pow(t0, 1.0-spend_time/(limit_time)) * pow(t1, spend_time/(limit_time));
    }
    cnt += thread_num * tasks_num;
    std::future<std::pair<ll, RndInfo>> threads[thread_num];
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
    if(awesome_score > good_score){
      awesome_score = good_score;
      best_score = good_score;
      update_values(best_change);
      memcpy(awesome, best, sizeof(awesome));
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }else if(exp((double)(best_score - good_score) / temp) > rnd(0,1024)/1024.0){
      best_score = good_score;
      update_values(best_change);
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }
    /*
    if(good_score < best_score){
      best_score = good_score;
      update_values(best_change);
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }
    */
  }
  best_score = awesome_score;
  memcpy(best, awesome, sizeof(best));
  rep(i, half_n) used_idx[i] = 0;
  rep(i, m) used_idx[best[i].idx]++;
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Time per loop: " << (spend_time-temp_time)/cnt << "\n";
  cerr << "Final Score: " << best_score << "\n";

  File::output_result(best);
}

}; // namespace solver


int main(){
  srand(time(NULL));
  File::read_values();
  Solver::solve();
}