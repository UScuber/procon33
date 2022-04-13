#include "library.hpp"
using namespace std;

constexpr double limit_time = 40.0;

#define isTestCase


vector<vector<Val_Type>> arrays[n];
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

void solve(){
  // 適当にはじめは値を入れておく
  int best_pos[m] = {};
  int best_select_idx[m];
  int used_idx[n] = {};
  rep(i, m){
    best_select_idx[i] = i;
    used_idx[i] = 1;
  }
  ll best_score = calc_selected_ans(best_select_idx, best_pos);
  
  cerr << "First Score: " << best_score << "\n";

  int update_num = 0;
  double last_upd_time = -1;
  int steps = 0;

  double spend_time = clock();
  const clock_t start_time = clock();
  // 山登り法
  for(; ; steps++){
    constexpr int mask = (1 << 8) - 1;
    if(!(steps & mask)){
      const clock_t end_time = clock();
      spend_time = clock() - start_time;
      spend_time /= CLOCKS_PER_SEC;
      if(spend_time > limit_time) break;
    }
    const int t = rnd(0, 10) >= 2;
    int idx = -1, pos, nxt_idx = -1;
    // select wav and change pos
    if(t == 0){
      idx = rnd(0, m);
      pos = rnd(0, tot_frame);
    }
    // select other wav and swap and change pos
    else{
      idx = rnd(0, m);
      nxt_idx = rnd(0, n);
      if(used_idx[nxt_idx]){
        steps--;
        continue;
      }
      pos = rnd(0, tot_frame);
    }
    assert(idx != -1);
    // 更新できなかった時に復元する
    const int tmp_idx = best_select_idx[idx];
    const int tmp_pos = best_pos[idx];

    // 値の仮代入
    if(t == 0){
      best_pos[idx] = pos;
    }
    else if(t == 1){
      best_select_idx[idx] = nxt_idx;
      best_pos[idx] = pos;
    }

    const ll score = calc_selected_ans(best_select_idx, best_pos);
    if(score < best_score){
      best_score = score;
      if(t == 1){
        used_idx[tmp_idx]--;
        used_idx[nxt_idx]++;
      }
      cerr << "u";
      update_num++;
      last_upd_time = spend_time;
      if(best_score == 0) break;
    }else{
      best_select_idx[idx] = tmp_idx;
      best_pos[idx] = tmp_pos;
    }
  }
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
  cerr << "Last Update: " << last_upd_time << "\n";
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


int main(){
  srand(time(NULL));
  cin.tie(nullptr);
  ios::sync_with_stdio(false);
  #ifdef isTestCase
    read();
  #else
    init();
  #endif
  solve();
}