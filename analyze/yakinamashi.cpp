#include "library.hpp"
#include "temp.hpp"

constexpr double limit_time = 123;


namespace Solver {

Data awesome[m];
Score_Type awesome_score = inf_score;

void solve(){
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  double temp = t0;
  double spend_time = 0, p = 0;
  StopWatch sw;
  if(contains_num == m) goto END;
  // 焼きなまし法
  for(; ; steps++){
    constexpr int mask = (1 << 10) - 1;
    if(!(steps & mask)){
      spend_time = sw.get_time();
      if(spend_time > limit_time) break;
      p = spend_time / limit_time;
      //temp = pow(t0, 1.0-p) * pow(t1, p);
      temp = (t1 - t0) * p + t0;
    }
    RndInfo change;
    if(p < 0.5) rnd_create_first(change);
    else rnd_create_second(change);
    const Score_Type score = calc_one_changed_ans2(change);
    if(awesome_score > score){
      awesome_score = score;
      best_score = score;
      update_values(change);
      memcpy(awesome, best, sizeof(awesome));
      std::cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }else if(fast_exp((double)(best_score - score) / temp) > rnd(1024)/1024.0){
      best_score = score;
      update_values(change);
    }
  }
  END:
  std::cerr << "\n";
  std::cerr << "Steps: " << steps << "\n";
  std::cerr << "Updated: " << update_num << "\n";
  std::cerr << "Last Update: " << last_upd_time << "\n";
  std::cerr << "Time per loop: " << spend_time/steps << "\n";
  std::cerr << "Final Score: " << awesome_score << "\n";
  
  File::output_result(best, awesome_score);
}

}; // namespace solver


int main(){
  srand(time(NULL));
  File::read_values();
  Solver::solve();
}