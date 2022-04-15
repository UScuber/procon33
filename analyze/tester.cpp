#include "library.hpp"
using namespace std;

void make_random(){
  assert(problem.size() == ans_length);
  rep(i, n){
    arrays[i].resize(tot_frame);
    rep(j, tot_frame){
      arrays[i][j] = make_rnd_array(dhz);
    }
  }
  // make answer_idx
  int used[n] = {};
  rep(i, m){
    const int ran = rnd(0, n);
    if(used[ran]){
      i--; continue;
    }
    answer_idx[i] = ran;
    used[ran] = 1;
  }
  rep(i, m){
    assert(0 <= answer_idx[i] && answer_idx[i] < n);
    const int pos = rnd(0, tot_frame);
    rep(j, tot_frame){
      add(problem[j + pos], arrays[answer_idx[i]][j]);
    }
    answer_pos[i] = pos;
  }
  // ランダムに値をずらす
  rep(i, ans_length){
    rep(j, dhz){
      const int t = rnd(0, 2);
      if(t) continue;
      problem[i][j] = rnd(7,14)/10.0 * problem[i][j];
    }
  }
}

// テストケースのn,mは常に一定の値なので試すときはそれに合わせる必要がある
int main(){
  cin.tie(nullptr);
  ios::sync_with_stdio(false);
  srand(time(NULL));
  make_random();
  // output testcase
  // arrays
  rep(i, n){
    rep(j, tot_frame){
      rep(k, dhz) cout << arrays[i][j][k] << " ";
    }
    cout << "\n";
  }
  rep(i, ans_length){
    rep(j, dhz) cout << problem[i][j] << " ";
  }
  cout << "\n";
  rep(i, m) cout << answer_idx[i] << " ";
  cout << "\n";
  rep(i, m) cout << answer_pos[i] << " ";
  cout << "\n";
}