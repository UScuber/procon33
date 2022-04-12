#include <bits/stdc++.h>
#define rep(i, n) for(int i = 0; i < (n); i++)
using ll = long long;
using uint = unsigned int;
using namespace std;

constexpr int n = 44; //candidate arrays
constexpr int m = 15; //select num
constexpr int fps = 30;
constexpr int tot_time = 5;
constexpr int tot_frame = fps * tot_time;
constexpr int dhz = 600;
constexpr int ans_length = tot_frame * 2;

constexpr double limit_time = 20.0;

vector<vector<int>> arrays[n];
vector<vector<int>> problem;
int answer_idx[m];
int answer_pos[m];

// libraries
uint randxor(){
  static uint x = rand() | rand() << 16;
  static uint y = rand() | rand() << 16;
  static uint z = rand() | rand() << 16;
  static uint w = rand() | rand() << 16;
  //static uint x=123456789,y=362436069,z=521288629,w=88675123;
  uint t;
  t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
}
// returns random [l, r)
inline int rnd(const int &l, const int &r){
  return randxor() % (r - l) + l;
}
const double PI = acos(-1);
vector<int> make_rnd_array(int n){
  const int first_hz = rnd(5, 10);
  vector<int> res(n);
  //rep(i, n) res[i] = randxor() >> 12;
  rep(i, n){
    double arc = cos(PI * i / first_hz) * 30;
    arc *= arc;
    res[i] = arc * rnd(7, 14)/10.0;
  }
  return res;
}
inline void add(vector<int> &a, const vector<int> &b){
  assert(b.size() <= a.size());
  rep(i, b.size()) a[i] += b[i];
}
inline void sub(vector<int> &a, const vector<int> &b){
  assert(b.size() <= a.size());
  rep(i, b.size()) a[i] -= b[i];
}

// problemから数字を引いたやつのスコアを計算する
ll calc_score(const vector<vector<int>> &a){
  double score = 0;
  rep(i, a.size()){
    ll tot = 0;
    rep(j, a[0].size()){
      //tot += a[i][j] * a[i][j];
      tot += abs(a[i][j]);
    }
    score += (double)tot / a.size();
  }
  return score;
}

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
  static_assert(m <= n);
  rep(i, n){
    arrays[i].resize(tot_frame);
    rep(j, tot_frame){
      arrays[i][j] = make_rnd_array(dhz);
    }
  }
  // make answer_idx
  int used[n] = {};
  rep(i, m){
    int ran = rnd(0, n);
    if(used[ran]){
      i--; continue;
    }
    answer_idx[i] = ran;
    used[ran] = 1;
  }
  problem.assign(ans_length, vector<int>(dhz));
  rep(i, m){
    assert(0 <= answer_idx[i] && answer_idx[i] < n);
    const int pos = rnd(0, tot_frame);
    rep(j, tot_frame){
      add(problem[j + pos], arrays[answer_idx[i]][j]);
    }
    answer_pos[i] = pos;

    // output answer_idx
    cout << answer_idx[i] << " " << pos << "\n";
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
  int steps = 0;

  const clock_t start_time = clock();
  // 山登り法
  for(; ; steps++){
    constexpr int mask = (1 << 8) - 1;
    if(!(steps & mask)){
      const clock_t end_time = clock();
      const double spend_time = (end_time - start_time) / CLOCKS_PER_SEC;
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
      cerr << "upd";
      update_num++;
      if(best_score == 0) break;
    }else{
      best_select_idx[idx] = tmp_idx;
      best_pos[idx] = tmp_pos;
    }
  }
  cerr << "\n";
  cerr << "Steps: " << steps << "\n";
  cerr << "Updated: " << update_num << "\n";
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
  init();
  // ランダムに値をずらす
  rep(i, ans_length){
    rep(j, dhz){
      const int t = rnd(0, 2);
      if(t) continue;
      problem[i][j] = rnd(7,14)/10.0 * problem[i][j];
    }
  }
  solve();

  cout << "\n";
  rep(i, tot_frame){
    rep(j, dhz){
      cout << arrays[0][i][j] << " ";
    }
    cout << "\n";
  }
}