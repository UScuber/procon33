#include "library.hpp"

constexpr double limit_time = 60.0;


namespace Solver {

Data awesome[m];
Score_Type awesome_score = inf_score;

void solve(){
  init();
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  constexpr double t0 = 2.666e3;
  constexpr double t1 = 1.0e2;
  double temp = t0;
  double spend_time = 0;
  StopWatch sw;
  int best_cnt[n] = {};
  // 焼きなまし法
  for(; ; steps++){
    constexpr int mask = (1 << 10) - 1;
    if(!(steps & mask)){
      spend_time = sw.get_time();
      if(spend_time > limit_time) break;
      /*
      // not recently updated
      if(spend_time-last_upd_time > 10.0){
        cerr << "r";
        last_upd_time = spend_time;
        best_score = awesome_score;
        init_array(awesome);
      }
      */
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
      rep(i, m) best_cnt[best[i].idx]++;
      last_upd_time = spend_time;
    }else if(fast_exp((double)(best_score - score) / temp) > rnd(1024)/1024.0){
      best_score = score;
      update_values(change);
      //cerr << "u";
      //last_upd_time = spend_time;
    }
  }
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Time per loop: " << spend_time/steps << "\n";
  cerr << "Final Score: " << awesome_score << "\n";
  rep(i, half_n) cerr << best_cnt[i] << " ";
  cerr << "\n";
  rep(i, half_n) cerr << best_cnt[i + half_n] << " ";
  cout << awesome_score << " ";
  File::output_result(best);
}

}; // namespace solver


int main(){
  srand(time(NULL));
  File::read_values();
  Solver::solve();
}