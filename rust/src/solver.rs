use rand::prelude::*;
use std::cmp::min;
use std::cmp::max;
use crate::file_reader::*;

const INF: i32 = i32::MAX;
const INFL: i64 = i64::MAX;
const HALF_N: usize = N / 2;
const HZ: usize = ANALYZE_SAMP_HZ; //sampling hz[48k->12k]
//const TOT_FRAME: i32 = ANALYZE_MAX_LEN; //max size of arrays[i]

//constexpr int ans_length = hz * 8;

// 数列の値の型
type Val_Type = i32;

//static problem: Lazy<[i32; ANS_LEN]> = Lazy::new(|| read_problem_audio());
//static problem_length: Lazy<usize> = Lazy::new(|| read_problem_length());
//static arrays: Lazy<[[i32; ANALYZE_MAX_LEN]; N]> = Lazy::new(|| read_audio_arrays());
//static audio_length: Lazy<[usize; N]> = Lazy::new(|| read_audio_length());

//int audio_length[n] = {};
//let audio_length = [0; n];
//Val_Type problem[ans_length] = {};
//int problem_length = ans_length;
//bool has_answer = true;

#[derive(Debug, Clone, Copy)]
struct Data {
  idx: usize, //札の種類
  pos: usize, //貼り付け位置
  st: usize, //札の再生開始位置
  len: usize, //札の再生の長さ
}
struct RndInfo {
  idx: usize, //変更する値
  pos: usize, //変更後の貼り付け位置
  nxt_idx: usize, //新しく更新する札
  st: usize, //次の札の再生開始位置
  len: usize, //次の札の再生の長さ
}
impl RndInfo {
  const fn new() -> Self {
    Self { idx:0, pos:0, nxt_idx:0, st:0, len:0 }
  }
}
//Data answer[m];


const fn weight(x: i32) -> i64 {
  //(x as i64) * (x as i64)
  x.abs() as i64
}

// 問題の数列から数字を引いたやつのスコアを計算する
fn calc_score(a: [Val_Type; ANS_LEN]) -> i64 {
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
  problem: [i32; ANS_LEN],
  problem_length: usize,
  arrays: [[i32; ANALYZE_MAX_LEN]; N],
  audio_length: [usize; N],

  best: [Data; M],
  used_idx: [bool; M],
  best_sub: [Val_Type; ANS_LEN],
  best_score: i64,
}
impl Solver {
  fn new() -> Self {
    let problem: [i32; ANS_LEN] = read_problem_audio();
    let problem_length: usize = read_problem_length();
    let arrays: [[i32; ANALYZE_MAX_LEN]; N] = read_audio_arrays();
    let audio_length: [usize; N] = read_audio_length();
    
    let mut best_sub: [Val_Type; ANS_LEN] = [0; ANS_LEN];
    for i in 0..ANS_LEN {
      best_sub[i] = problem[i];
    }
    let mut best: [Data; M] = [Data{idx:0,pos:0,st:0,len:0}; M];
    let mut used_idx = [false; M];
    for i in 0..M {
      best[i].idx = i;
      best[i].len = min(problem_length, audio_length[best[i].idx]);
      used_idx[i] = true;
      for j in 0..best[i].len {
        best_sub[j + best[i].pos] -= arrays[best[i].idx][j + best[i].st];
      }
    }
    let best_score = calc_score(best_sub);
    println!("First Score: {}", best_score);
    Self {
      problem, problem_length, arrays, audio_length,
      best, used_idx, best_sub, best_score,
    }
  }

  fn rnd_create(&self, change: &mut RndInfo, rng: &mut ThreadRng){
    let t = rng.gen_range(0..10) >= 2;
    if t == false {
      change.idx = rng.gen_range(0..M);
      change.nxt_idx = self.best[change.idx].idx;
      change.len = rng.gen_range(HZ..=min(self.problem_length, self.audio_length[change.nxt_idx]));
      change.st = rng.gen_range(0..=self.audio_length[change.nxt_idx]-change.len);
      change.pos = rng.gen_range(0..=self.problem_length-change.len);
    }
    else{
      change.idx = rng.gen_range(0..M);
      change.nxt_idx = rng.gen_range(0..N);
      while self.used_idx[change.nxt_idx % HALF_N] && self.best[change.idx].idx % HALF_N != change.nxt_idx % HALF_N {
        change.nxt_idx = rng.gen_range(0..N);
      }
      change.len = rng.gen_range(HZ..=min(self.problem_length, self.audio_length[change.nxt_idx]));
      change.st = rng.gen_range(0..=self.audio_length[change.nxt_idx]-change.len);
      change.pos = rng.gen_range(0..=self.problem_length-change.len);
    }
  }
  fn rnd_create2(&self, rng: &mut ThreadRng) -> RndInfo {
    let mut change = RndInfo::new();
    self.rnd_create(&mut change, rng);
    return change;
  }
  fn calc_one_changed_ans(&self, info: RndInfo) -> i64 {
    let pre: &Data = &self.best[info.idx];
    let info_rig = info.pos + info.len;
    let pre_rig = pre.pos + pre.len;
    let mut score = self.best_score;
    if max(info.pos, pre.pos) < min(info_rig, pre_rig) {
      // 左側がleft
      if info.pos <= pre.pos {
        let rightest = min(info_rig, pre.pos);
        for i in 0..rightest-info.pos {
          score += weight(self.best_sub[i + info.pos] - self.arrays[info.nxt_idx][i + info.st]) - weight(self.best_sub[i + info.pos]);
        }
      }
      // 左側がright
      else{
        let rightest = min(pre_rig, info.pos);
        for i in 0..rightest-pre.pos {
          score += weight(self.best_sub[i + pre.pos] + self.arrays[pre.idx][i + info.st]) - weight(self.best_sub[i + pre.pos]);
        }
      }
      // 右側がright
      if info_rig <= pre_rig {
        let leftest = max(pre.pos, info_rig);
        let s = max(info_rig - pre.pos, 0);
        for i in 0..pre_rig-leftest {
          score += weight(self.best_sub[i + leftest] + self.arrays[pre.idx][i + s + pre.st]) - weight(self.best_sub[i + leftest]);
        }
      }
      // 右側がleft
      else{
        let leftest = max(info.pos, pre_rig);
        let s = max(pre_rig - info.pos, 0);
        for i in 0..info_rig-leftest {
          score += weight(self.best_sub[i + leftest] - self.arrays[info.nxt_idx][i + s + info.st]) - weight(self.best_sub[i + leftest]);
        }
      }
      // middle
      let leftest = max(info.pos, pre.pos);
      let range = min(info_rig, pre_rig) - leftest;
      let sl = max(pre.pos - info.pos, 0);
      let sr = max(info.pos - pre.pos, 0);
      for i in 0..range {
        score -= weight(self.best_sub[i + leftest]);
        let d: Val_Type = self.best_sub[i + leftest] - self.arrays[info.nxt_idx][i + sl + info.st] + self.arrays[pre.idx][i + sr + pre.st];
        score += weight(d);
      }
    }
    else{
      for i in 0..info.len {
        score += weight(self.best_sub[i + info.pos] - self.arrays[info.nxt_idx][i + info.st]) - weight(self.best_sub[i + info.pos]);
      }
      for i in 0..pre.len {
        score += weight(self.best_sub[i + pre.pos] + self.arrays[pre.idx][i + pre.st]) - weight(self.best_sub[i + pre.pos]);
      }
    }
    return score;
  }
}

