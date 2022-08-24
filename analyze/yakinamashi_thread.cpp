#include <thread>
#include <future>
#include "library.hpp"

constexpr double limit_time = 60.0 * 5;


namespace Solver {

constexpr int thread_num = 12 + 4;
constexpr int max_tasks_num = 512 * 8 / 4 / 2;
int tasks_num = max_tasks_num;

RndInfo rnd_arrays[thread_num][max_tasks_num];

Data awesome[m];
Score_Type awesome_score = inf_score;

std::pair<Score_Type, int> solve_one_thread(const RndInfo ran[max_tasks_num]){
  Score_Type best_sc = inf_score;
  int best_change_idx = -1;
  rep(i, tasks_num){
    const Score_Type score = calc_one_changed_ans(ran[i]);
    if(best_sc > score){
      best_sc = score;
      best_change_idx = i;
    }
  }
  return { best_sc, best_change_idx };
}

void solve(){
  init();
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  constexpr double t0 = 4e3/1.5;
  constexpr double t1 = 1.2e2/1.25;
  double temp = t0;
  StopWatch sw;
  double spend_time = 0;
  // 焼きなまし法(single thread)
  cerr << "Start Single Thread\n";
  for(; ; steps++){
    constexpr int mask = (1 << 9) - 1;
    if(!(steps & mask)){
      spend_time = sw.get_time();
      if(spend_time > limit_time*0.60) break;
      temp = pow(t0, 1.0-spend_time/limit_time) * pow(t1, spend_time/limit_time);
    }
    RndInfo change;
    rnd_create(change);
    const Score_Type score = calc_one_changed_ans(change);
    if(awesome_score > score){
      awesome_score = score;
      best_score = score;
      update_values(change);
      memcpy(awesome, best, sizeof(awesome));
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }else if(fast_exp((double)(best_score - score) / temp) > rnd(1024)/1024.0){
      best_score = score;
      update_values(change);
      //cerr << "u";
      update_num++;
      //last_upd_time = spend_time;
    }
  }
  best_score = awesome_score;
  init_array(awesome);

  cerr << "\n";
  cerr << "Score: " << best_score << "\n";
  cerr << "Start Multi Thread\n";
  // 山登り法(multi thread)
  double temp_time = spend_time;
  int cnt = 0;
  for(; ; steps += thread_num * tasks_num){
    {
      spend_time = sw.get_time();
      if(spend_time > limit_time) break;
      temp = pow(t0, 1.0-spend_time/limit_time) * pow(t1, spend_time/limit_time);
      if(spend_time - last_upd_time >= 1.0){
        tasks_num = max_tasks_num;
      }else{
        tasks_num = max_tasks_num / 12;
      }
    }
    cnt += thread_num * tasks_num;
    std::future<std::pair<Score_Type, int>> threads[thread_num];
    rep(i, thread_num){
      rep(j, tasks_num){
        if(spend_time - last_upd_time <= 3.0)
          rnd_create(rnd_arrays[i][j]);
        else
          rnd_create2(rnd_arrays[i][j]);
      }
      threads[i] = std::async(solve_one_thread, rnd_arrays[i]);
    }
    int best_change_idx = -1;
    Score_Type good_score = inf_score;
    rep(i, thread_num){
      Score_Type score; int idx;
      std::tie(score, idx) = threads[i].get();
      if(good_score > score){
        good_score = score;
        best_change_idx = i*max_tasks_num + idx;
      }
    }
    const RndInfo &best_change = *(*rnd_arrays + best_change_idx);
    if(awesome_score > good_score){
      awesome_score = good_score;
      best_score = good_score;
      update_values(best_change);
      memcpy(awesome, best, sizeof(awesome));
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }else if(exp((double)(best_score - good_score) / temp) > rnd(1024)/1024.0){
      best_score = good_score;
      update_values(best_change);
      //cerr << "u";
      //last_upd_time = spend_time;
    }
  }
  best_score = awesome_score;
  init_array(awesome);
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Time per loop: " << (spend_time-temp_time)/cnt << "\n";
  cerr << "Final Score: " << best_score << "\n";

  cout << awesome_score << " ";
  File::output_result(best);
}

}; // namespace solver


int main(){
  srand(time(NULL));
  File::read_values();
  Solver::solve();
}