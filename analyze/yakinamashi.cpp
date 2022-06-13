#include "library.hpp"

constexpr double limit_time = 60.0 * 5;


void read(){
  File::read_values(cin);
  // output answer_idx
  if(has_answer) rep(i, m){
    if(answer[i].idx < half_n) cout << "J" << answer[i].idx+1;
    else cout << "E" << answer[i].idx-44+1;
    cout << "\n";
  }
  cout << "\n";
}

namespace solver {

Data awesome[m];
ll awesome_score = infl;

void solve(){
  init();
  awesome_score = best_score;
  memcpy(awesome, best, sizeof(best));

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  constexpr double t0 = 1e4;
  constexpr double t1 = 4e2;
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
    const RndInfo change = rnd_create();
    const ll score = calc_one_changed_ans(change);
    if(awesome_score < score){
      awesome_score = score;
      best_score = score;
      update_values(change);
      memcpy(awesome, best, sizeof(awesome));
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }else if(exp((double)(score - best_score) / temp) > rnd(0,1024)/1024.0){
      best_score = score;
      update_values(change);
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
    }
  }
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Time per loop: " << spend_time/steps << "\n";
  cerr << "Final Score: " << awesome_score << "\n";

  // output result
  rep(i, m){
    if(awesome[i].idx < half_n) cout << "J" << awesome[i].idx+1;
    else cout << "E" << awesome[i].idx-44+1;
    cout << " " << awesome[i].pos * 4 << "\n";
  }
  if(has_answer){
    cout << "\n";
    int diff_num = 0;
    rep(i, m){
      bool ok = false;
      rep(j, m){
          if(awesome[i].idx == answer[j].idx){
          ok = true; break;
        }
      }
      if(ok) continue;
      diff_num++;
      if(awesome[i].idx < half_n) cout << "J" << awesome[i].idx+1;
      else cout << "E" << awesome[i].idx-44+1;
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