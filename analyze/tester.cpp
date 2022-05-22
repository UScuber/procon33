#include "library.hpp"
using namespace std;

void make_random(){
  rep(i, n){
    rep(j, tot_frame){
      make_rnd_array(arrays[i][j]);
    }
  }
  // make answer_idx
  int used[n] = {};
  rep(i, m){
    const int ran = rnd(0, n);
    if(used[ran]){
      i--; continue;
    }
    answer[i].idx = ran;
    used[ran] = 1;
  }
  rep(i, m){
    assert(0 <= answer[i].idx && answer[i].idx < n);
    const int pos = rnd(0, tot_frame);
    const int st = rnd(0, tot_frame - fps);
    const int len = rnd(fps, tot_frame - st + 1);
    rep(j, len){
      add(problem[j + pos], arrays[answer[i].idx][j + st]);
    }
    answer[i].pos = pos;
    answer[i].st = st;
    answer[i].len = len;
  }
  // ランダムに値をずらす
  rep(i, ans_length){
    rep(j, dhz){
      const int t = rnd(0, 100);
      if(t < 4) problem[i][j] = problem[i][j] / 5;
      else if(t < 7) problem[i][j] = problem[i][j] * 3/10.0;
      else if(t < 10) problem[i][j] = problem[i][j] * rnd(4,6)/10.0;
      else if(t < 20) problem[i][j] = problem[i][j] * rnd(6,8)/10.0;
      else if(t < 27) problem[i][j] = problem[i][j] * rnd(8,11)/10.0;
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
  rep(i, m) cout << answer[i].idx << " ";
  cout << "\n";
  rep(i, m) cout << answer[i].pos << " ";
  cout << "\n";
  rep(i, m) cout << answer[i].st << " ";
  cout << "\n";
  rep(i, m) cout << answer[i].len << " ";
  cout << "\n";
}