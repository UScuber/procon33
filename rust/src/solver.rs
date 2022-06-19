use once_cell::sync::Lazy;
use rand::distributions::uniform::SampleRange;
use rand::prelude::*;
use std::array;
use std::cmp::min;
use std::cmp::max;
use crate::file_reader::*;

const INF: i32 = i32::MAX;
const INFL: i64 = i64::MAX;
const HALF_N: usize = N / 2;
const HZ: i32 = ANALYZE_SAMP_HZ; //sampling hz[48k->12k]
//const TOT_FRAME: i32 = ANALYZE_MAX_LEN; //max size of arrays[i]

//constexpr int ans_length = hz * 8;

// 数列の値の型
type Val_Type = i32;

static problem: [i32; ANS_LEN] = Lazy::new(|| read_problem_audio());
static problem_length: usize = Lazy::new(|| read_problem_length());
static arrays: [[i32; ANALYZE_MAX_LEN]; N] = Lazy::new(|| read_audio_arrays());
static audio_length: [usize; N] = Lazy::new(|| read_audio_length());

//int audio_length[n] = {};
//let audio_length = [0; n];
//Val_Type problem[ans_length] = {};
//int problem_length = ans_length;
//bool has_answer = true;

struct Data {
  pub idx: i32, //札の種類
  pub pos: i32, //貼り付け位置
  pub st: i32, //札の再生開始位置
  pub len: i32, //札の再生の長さ
}
struct RndInfo {
  pub idx: usize, //変更する値
  pub pos: i32, //変更後の貼り付け位置
  pub nxt_idx: i32, //新しく更新する札
  pub st: i32, //次の札の再生開始位置
  pub len: i32, //次の札の再生の長さ
}
//Data answer[m];

/*
inline uint randxor32() noexcept{
  static uint x = rand() | rand() << 16;
  x ^= x << 13; x ^= x >> 17;
  return x ^= x << 5;
}
// returns random [l, r)
inline int rnd(const int &l, const int &r) noexcept{
  return randxor128() % (r - l) + l;
}
#define Weight(x) abs(x)
//#define Weight(x) ((ll)(x)*(ll)(x))
*/

const fn weight(x: i32) -> i32 {
  //(x as i64) * (x as i64)
  x.abs()
}

// 問題の数列から数字を引いたやつのスコアを計算する
const fn calc_score(a: [Val_Type; ANS_LEN]) -> i64 {
  let mut score: i64 = 0;
  for i in 0..ANS_LEN {
    score += weight(a[i]);
  }
  return score;
}


/*
namespace File {

void read_values(){
  rep(i, n){
    audio_length[i] = tot_frame;
    rep(j, tot_frame){
      if(arrays[i][j] == inf){
        audio_length[i] = j;
        break;
      }
    }
  }
  rep(i, ans_length){
    std::cin >> problem[i];
    if(problem[i] == inf){
      problem_length = i;
      problem[i] = 0;
      break;
    }
  }
  rep(i, m) std::cin >> answer[i].idx;
  if(cin.eof()) has_answer = false;

  // output answer_idx
  if(has_answer) rep(i, m){
    if(answer[i].idx < half_n) cout << "J" << answer[i].idx+1;
    else cout << "E" << answer[i].idx-half_n+1;
    cout << "\n";
  }
  cout << "\n";
}

void output_result(const Data best[m]){
  rep(i, m){
    if(best[i].idx < half_n) cout << "J" << best[i].idx+1;
    else cout << "E" << best[i].idx-half_n+1;
    cout << " " << best[i].pos * 4 << "\n";
  }
  if(has_answer){
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
      if(best[i].idx < half_n) cout << "J" << best[i].idx+1;
      else cout << "E" << best[i].idx-half_n+1;
      cout << " ";
    }
    cerr << "Diff: " << diff_num << "/" << m << "\n";
  }
}

}; // namespace File
*/


struct Solver {
  best: [Data; M],
  used_idx: [bool; M],
  best_sub: [Val_Type; ANS_LEN],
  best_score: i64,
  //temp

}
impl Solver {
  const fn new() -> Self {
    let mut best_sub = problem;
    let mut best = [Data{idx:0,pos:0,st:0,len:0}; M];
    let mut used_idx = [false; M];
    for i in 0..M {
      best[i].idx = i;
      best[i].len = min(problem_length, audio_length[best[i].idx]);
      used_idx[i] = 1;
      for j in 0..best[i].len {
        best_sub[j + best[i].pos] -= arrays[best[i].idx][j + best[i].st];
      }
    }
    let best_score = calc_score(best_sub);
    println!("First Score: {}", best_score);
    Self {
      best,
      used_idx,
      best_sub,
      best_score,
    }
  }

  const fn rnd_create(&self, change: &mut RndInfo, rng: &mut ThreadRng){
    let t = rng.gen_range(0..10) >= 2;
    if t == 0 {
      change.idx = rng.gen_range(0..m);
      change.nxt_idx = self.best[change.idx].idx;
      change.len = rng.gen_range(hz..=min(problem_length, audio_length[change.nxt_idx]));
      change.st = rng.gen_range(0..=audio_length[change.nxt_idx]-change.len);
      change.pos = rng.gen_range(0..=problem_length-change.len);
    }
    else{
      change.idx = rng.gen_range(0..m);
      change.nxt_idx = rng.gen_range(0..N);
      while self.used_idx[change.nxt_idx % HALF_N] && self.best[change.idx].idx % HALF_N != change.nxt_idx % HALF_N {
        change.nxt_idx = rng.gen_range(0..N);
      }
      change.len = rng.gen_range(hz..=min(problem_length, audio_length[change.nxt_idx]));
      change.st = rng.gen_range(0..=audio_length[change.nxt_idx]-change.len);
      change.pos = rng.gen_range(0..=problem_length-change.len);
    }
  }
  const fn rnd_create(&self, rng: &mut ThreadRng) -> RndInfo {
    let mut change = RndInfo;
    self.rnd_create(change, rng);
    return change;
  }
  const fn calc_range_score_sub(&self, arr_idx: usize, info: RndInfo, range: usize, score: &mut i64){
    for i in 0..range {
      score += weight(self.best_sub[i + info.pos] - arrays[arr_idx][i + info.st]) - weight(self.best_sub[i + info.pos]);
    }
  }
  const fn calc_range_score_add(&self, arr_idx: usize, info: RndInfo, range: usize, score: &mut i64){
    for i in 0..range {
      score += weight(self.best_sub[i + info.pos] + arrays[arr_idx][i + info.st]) - weight(self.best_sub[i + info.pos]);
    }
  }
  const fn calc_one_changed_ans(&self, info: RndInfo) -> i64 {
    let pre = self.best[info.idx];
    let info_rig = info.pos + info.len;
    let pre_rig = pre.pos + pre.len;
    let mut score = self.best_score;
    if max(info.pos, pre.pos) < min(info_rig, pre_rig) {
      // 左側がleft
      if info.pos <= pre.pos {
        let rightest = min(info_rig, pre.pos);
      }
      // 左側がright
      else{
        let rightest = min(pre_rig, info.pos);
      }
      // 右側がright
      if info_rig <= pre_rig {
        
      }
      // 右側がleft
      else{

      }
      // middle
    }
    else{

    }
  }
}
inline constexpr void calc_range_score_sub(const Val_Type a[], const Val_Type b[], const int &range, ll &score) noexcept{
  rep(i, range) score += Weight(b[i] - a[i]) - Weight(b[i]);
}
inline constexpr void calc_range_score_add(const Val_Type a[], const Val_Type b[], const int &range, ll &score) noexcept{
  rep(i, range) score += Weight(b[i] + a[i]) - Weight(b[i]);
}

inline constexpr ll calc_one_changed_ans(const RndInfo &info) noexcept{
  const Data &pre = best[info.idx];
  const int info_rig = info.pos + info.len;
  const int pre_rig = pre.pos + pre.len;
  const Val_Type *arr_info = arrays[info.nxt_idx] + info.st;
  const Val_Type *arr_pre = arrays[pre.idx] + pre.st;
  ll score = best_score;
  // cross
  if(max(info.pos, pre.pos) < min(info_rig, pre_rig)){
    // 左側がleft
    if(info.pos <= pre.pos){
      const int rightest = min(info_rig, pre.pos);
      calc_range_score_sub(arr_info, best_sub+info.pos, rightest-info.pos, score);
    }
    // 左側がright
    else{
      const int rightest = min(pre_rig, info.pos);
      calc_range_score_add(arr_pre, best_sub+pre.pos, rightest-pre.pos, score);
    }
    // 右側がright
    if(info_rig <= pre_rig){
      const int leftest = max(pre.pos, info_rig);
      const int s = max(info_rig - pre.pos, 0);
      calc_range_score_add(arr_pre+s, best_sub+leftest, pre_rig-leftest, score);
    }
    // 右側がleft
    else{
      const int leftest = max(info.pos, pre_rig);
      const int s = max(pre_rig - info.pos, 0);
      calc_range_score_sub(arr_info+s, best_sub+leftest, info_rig-leftest, score);
    }
    // middle
    const int leftest = max(info.pos, pre.pos);
    const int range = min(info_rig, pre_rig) - leftest;
    const int sl = max(pre.pos - info.pos, 0);
    const int sr = max(info.pos - pre.pos, 0);
    rep(i, range){
      score -= Weight(best_sub[i+leftest]);
      const Val_Type d = best_sub[i+leftest] - arr_info[i+sl] + arr_pre[i+sr];
      score += Weight(d);
    }
  }
  // not cross
  else{
    calc_range_score_sub(arr_info, best_sub+info.pos, info.len, score);
    calc_range_score_add(arr_pre, best_sub+pre.pos, pre.len, score);
  }
  return score;
}


fn update_values(info: RndInfo){
  let &pre: Data = best[info.idx];
  for i in 0..pre.len {
    
  }
}

void update_values(const RndInfo &info){
  // update best_sub
  const Data &pre = best[info.idx];
  rep(i, pre.len){
    best_sub[i + pre.pos] += arrays[pre.idx][i + pre.st];
  }
  rep(i, info.len){
    best_sub[i + info.pos] -= arrays[info.nxt_idx][i + info.st];
  }
  // update info
  used_idx[best[info.idx].idx % half_n]--;
  used_idx[info.nxt_idx % half_n]++;
  best[info.idx].idx = info.nxt_idx;
  best[info.idx].pos = info.pos;
  best[info.idx].st = info.st;
  best[info.idx].len = info.len;
}

} // namespace solver
