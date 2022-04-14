#include "library.hpp"
using namespace std;

constexpr double limit_time = 30.0;

#define isTestCase


vector<vector<Val_Type>> problem(ans_length, vector<Val_Type>(dhz));
int answer_idx[m];
int answer_pos[m];


// 答えと仮定したやつを引いて、計算する
ll calc_selected_ans(const int ans[m], const int pos[m]){
  auto temp = problem;
  rep(i, m){
    rep(j, tot_frame){
      sub(temp[j + pos[i]], arrays[ans[i]][j]);
    }
  }
  return calc_score(temp);
}

void init(){
  TestCase::make_random(arrays, problem, answer_idx, answer_pos);
  // output answer_idx
  rep(i, m){
    cout << answer_idx[i] << " " << answer_pos[i] << "\n";
  }
  cout << "\n";
}
void read(){
  File::read_values(arrays, problem, answer_idx, answer_pos, cin);
  // output answer_idx, pos
  rep(i, m){
    cout << answer_idx[i] << " " << answer_pos[i] << "\n";
  }
  cout << "\n";
}

namespace solver {

void solve(){
  best_sub = problem;
  // 最初は適当に値を入れておく
  rep(i, m){
    best_select_idx[i] = i;
    used_idx[i] = 1;
    // best_subの計算
    rep(j, tot_frame){
      sub(best_sub[j], arrays[i][j]);
    }
  }
  ll best_score = calc_score(best_sub);
  
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
    RndInfo change = rnd_create();
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
    cout << best_select_idx[i] << " " << best_pos[i] << "\n";
  }
  cout << "\n";
  int diff_num = 0;
  rep(i, m){
    bool ok = false;
    rep(j, m) if(best_select_idx[i] == answer_idx[j] && best_pos[i] == answer_pos[j]){
      ok = true; break;
    }
    if(ok) continue;
    diff_num++;
    cout << best_select_idx[i] << " " << best_pos[i] << "\n";
  }
  cerr << "Diff: " << diff_num << "/" << m << "\n";
}

}; // namespace solver


int main(){
  srand(time(NULL));
  cin.tie(nullptr);
  ios::sync_with_stdio(false);
  #ifdef isTestCase
    read();
  #else
    init();
  #endif
  solver::solve();
}