#include "library.h"

const double limit_time = (180.0/17*(m-3) + 45) * 1;

#define mask 1023

Data awesome[m];
Score_Type awesome_score = inf_score;

void solve(){
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  const double t0 = 2.5e3 * analyze_sampling_hz / 6000.0;
  const double t1 = 1.0e2 * analyze_sampling_hz / 6000.0;
  double temp = t0;
  double spend_time = 0, p = 0;
  const clock_t start_time = clock();
  if(contains_num == m) goto END;
  // 焼きなまし法
  for(; ; steps++){
    if(!(steps & mask)){
      spend_time = (clock() - start_time) * 1e-3;
      if(spend_time > limit_time) break;
      p = spend_time / limit_time;
      //temp = pow(t0, 1.0-p) * pow(t1, p);
      temp = (t1 - t0) * p + t0;
    }
    RndInfo change;
    if(p < 0.5) rnd_create_first(&change);
    else rnd_create_second(&change);
    const Score_Type score = calc_one_changed_ans2(&change);
    if(awesome_score > score){
      awesome_score = score;
      best_score = score;
      update_values(&change);
      memcpy(awesome, best, sizeof(awesome));
      fprintf(stderr, "u");
      update_num++;
      last_upd_time = spend_time;
    }else if(fast_exp((double)(best_score - score) / temp) > rnd2(1024)/1024.0){
      best_score = score;
      update_values(&change);
    }
  }
  END:
  fprintf(stderr, "\nSteps: %d\n", steps);
  fprintf(stderr, "Updated: %d\n", update_num);
  fprintf(stderr, "Last Update: %lf\n", last_upd_time);
  fprintf(stderr, "Time per loop: %lf\n", spend_time/steps);
  fprintf(stderr, "Final Score: %d\n", awesome_score);
  
  output_result(best, awesome_score);
}


int main(){
  arrays_initializer();
  srand(time(NULL));
  read_values();
  solve();
}