#include "library.hpp"

constexpr double limit_time = 60.0 * 5;


namespace Solver {

Data awesome[m];
ll awesome_score = infl;

void solve(){
  init();
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  constexpr double t0 = 4e3;
  constexpr double t1 = 1.2e2;
  double temp = t0;
  double spend_time = 0;
  const clock_t start_time = clock();
  // 山登り法
  for(; ; steps++){
    constexpr int mask = (1 << 7) - 1;
    if(!(steps & mask)){
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time) break;
      temp = pow(t0, 1.0-spend_time/limit_time) * pow(t1, spend_time/limit_time);
    }
    RndInfo change;
    rnd_create(change);
    const ll score = calc_one_changed_ans(change);
    if(awesome_score > score){
      awesome_score = score;
      best_score = score;
      update_values(change);
      memcpy(awesome, best, sizeof(awesome));
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }else if(exp((double)(best_score - score) / temp) > rnd(1024, seed)/1024.0){
      best_score = score;
      update_values(change);
      //cerr << "u";
      last_upd_time = spend_time;
    }
  }
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Time per loop: " << spend_time/steps << "\n";
  cerr << "Final Score: " << awesome_score << "\n";

  File::output_result(best);
}

}; // namespace solver


int main(){
  srand(time(NULL));
  File::read_values();
  Solver::solve();
}