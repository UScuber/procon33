#include "library.hpp"
using namespace std;

constexpr double limit_time = 60.0 / 6;


void read(){
  File::read_values(cin);
  // output answer_idx, pos
  rep(i, m){
    cout << answer[i].idx << " " << answer[i].pos << "\n";
  }
  cout << "\n";
}

namespace solver {

void solve(){
  //best_sub = problem;
  memcpy(best_sub, problem, sizeof(Val_Type) * ans_length * dhz);
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
  best_score = calc_score(best_sub);
  
  cerr << "First Score: " << best_score << "\n";

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  double spend_time = 0;
  const clock_t start_time = clock();
  // 山登り法
  for(; ; steps++){
    constexpr int mask = (1 << 7) - 1;
    if(!(steps & mask)){
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time) break;
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
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
  cerr << "Time per loop: " << spend_time/steps << "\n";
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