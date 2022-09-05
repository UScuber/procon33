#include <thread>
#include <future>
#include <omp.h>
#define USE_MULTI_THREAD
#include "library.hpp"

//constexpr double limit_time = 60.0 * 3;
constexpr double limit_time = 135.0/17*(m-3) + 45;

// Compile:
// $ g++ yakinamashi_thread.cpp -Ofast -fopenmp -lgomp

namespace Solver {

constexpr int thread_num = 12 + 4;
constexpr int max_tasks_num = 4096 * 16;
int tasks_num = max_tasks_num;

RndInfo rnd_arrays[thread_num * max_tasks_num];

Data awesome[m];
Score_Type awesome_score = inf_score;

struct AnalyzeResult {
  Score_Type score;
  int idx;
};

AnalyzeResult threads[thread_num];

void solve(){
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  double t0 = 2.5e3 * m / 20.0 * problem_length/(hz*7.5);
  double t1 = 1.0e2 * m / 20.0 * problem_length/(hz*7.5);
  double temp = t0;
  StopWatch sw;
  double spend_time = 0, p = 0;
  int cnt = 0;
  double temp_time = -1;
  int best_cnt[n] = {};
  if(contains_num == m) goto END;
  // 焼きなまし法(single thread)
  cerr << "Start Single Thread\n";
  for(; ; steps++){
    constexpr int mask = (1 << 9) - 1;
    if(!(steps & mask)){
      spend_time = sw.get_time();
      if(spend_time > limit_time*0.0) break;
      p = spend_time / limit_time;
      temp = pow(t0, 1.0-p) * pow(t1, p);
      //temp = (t1 - t0) * p + t0;
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
    }
  }
  best_score = awesome_score;
  init_array(awesome);

  cerr << "\n";
  cerr << "Score: " << best_score << "\n";
  cerr << "Start Multi Thread\n";
  // 山登り法(multi thread)
  temp_time = spend_time;
  for(; ; steps += thread_num * tasks_num){
    {
      spend_time = sw.get_time();
      if(spend_time > limit_time) break;
      p = spend_time / limit_time;
      temp = pow(t0, 1.0-p) * pow(t1, p);
      //temp = (t1 - t0) * p + t0;
      if(spend_time-last_upd_time > 10.0){
        cerr << "r";
        last_upd_time = spend_time;
        best_score = awesome_score;
        init_array(awesome);
      }
      if(spend_time - last_upd_time >= 1.0){
        tasks_num = max_tasks_num;
      }else{
        tasks_num = max_tasks_num >> 4;
      }
    }
    cnt += thread_num * tasks_num;

    const int calc_num = thread_num * tasks_num;
    if(spend_time - last_upd_time <= 2.0 || !rnd(10)){
      rep(i, calc_num) rnd_create(rnd_arrays[i]);
    }else{
      rep(i, calc_num) rnd_create2(rnd_arrays[i]);
    }
    // multi thread
    #pragma omp parallel for
    rep(i, thread_num){
      threads[i].score = inf_score;
      rep(j, tasks_num){
        const Score_Type score = calc_one_changed_ans(rnd_arrays[i*tasks_num + j]);
        if(threads[i].score > score){
          threads[i].score = score;
          threads[i].idx = j;
        }
      }
    }
    int best_change_idx = -1;
    Score_Type good_score = inf_score;
    rep(i, thread_num){
      if(good_score > threads[i].score){
        good_score = threads[i].score;
        best_change_idx = i*tasks_num + threads[i].idx;
      }
    }
    const RndInfo &best_change = rnd_arrays[best_change_idx];
    if(awesome_score > good_score){
      awesome_score = good_score;
      best_score = good_score;
      update_values(best_change);
      memcpy(awesome, best, sizeof(awesome));
      cerr << "u";
      update_num++;
      if(p >= 0.5) rep(i, m) best_cnt[best[i].idx]++;
      last_upd_time = spend_time;
    }else if(fast_exp((double)(best_score - good_score) / temp) > rnd(1024)/1024.0){
      best_score = good_score;
      update_values(best_change);
    }
  }
  END:
  best_score = awesome_score;
  init_array(awesome);
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Time per loop: " << (spend_time-temp_time)/cnt << "\n";
  cerr << "Final Score: " << best_score << "\n";
  rep(i, half_n) cerr << best_cnt[i] << " ";
  cerr << "\n";
  rep(i, half_n) cerr << best_cnt[i + half_n] << " ";
  cerr << "\n";
  vector<std::pair<int,int>> v(n);
  rep(i, n) v[i] = { best_cnt[i], i };
  std::sort(v.rbegin(), v.rend());
  int top[n] = {};
  rep(i, m) top[v[i].second] = 1;
  rep(i, half_n){
    if(top[i]) cerr << "\x1b[1m";
    bool ok = false;
    rep(j, m) if(answer[j].idx == i){
      ok = true; break;
    }
    rep(j, m) if(best[j].idx == i){
      cerr << "\x1b[42m";
      break;
    }
    if(ok) cerr << "\x1b[31m";
    cerr << best_cnt[i];
    cerr << "(J" << i+1 << ")";
    cerr << "\x1b[49m"; // background color
    cerr << "\x1b[39m"; // font color
    cerr << "\x1b[0m"; // under bar
    cerr << " ";
  }
  cerr << "\n";
  rep(i, half_n){
    if(top[i+half_n]) cerr << "\x1b[1m";
    rep(j, m) if(answer[j].idx == i+half_n){
      cerr << "\x1b[31m";
      break;
    }
    rep(j, m) if(best[j].idx == i+half_n){
      cerr << "\x1b[42m";
      break;
    }
    cerr << best_cnt[i + half_n];
    cerr << "(E" << i+1 << ")";
    cerr << "\x1b[49m"; // background color
    cerr << "\x1b[39m"; // font color
    cerr << "\x1b[0m"; // under bar
    cerr << " ";
  }
  File::output_result(best, awesome_score);
}

}; // namespace solver


int main(){
  srand(time(NULL));
  File::read_values();
  Solver::solve();
}