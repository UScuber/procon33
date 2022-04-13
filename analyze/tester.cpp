#include "library.hpp"
using namespace std;

vector<vector<Val_Type>> arrays[n];
vector<vector<Val_Type>> problem(ans_length, vector<Val_Type>(dhz));
int answer_idx[m];
int answer_pos[m];


// テストケースのn,mは常に一定の値なので試すときはそれに合わせる必要がある
int main(){
  cin.tie(nullptr);
  ios::sync_with_stdio(false);
  srand(time(NULL));
  TestCase::make_random(arrays, problem, answer_idx, answer_pos);
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