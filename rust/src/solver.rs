use rand::prelude::*;
use std::cmp::min;
use std::cmp::max;
use std::io::stdin;
use crate::file_reader::*;

#[allow(dead_code)]
const INF: i32 = i32::MAX;
#[allow(dead_code)]
const INFL: i64 = i64::MAX;
const HALF_N: usize = N / 2;
const HZ: usize = ANALYZE_SAMP_HZ; //sampling hz[48k->12k]
//const TOT_FRAME: i32 = ANALYZE_MAX_LEN; //max size of arrays[i]

// 数列の値の型
type ValType = i32;

#[derive(Debug, Clone, Copy)]
pub struct Data {
  idx: usize, //札の種類
  pos: usize, //貼り付け位置
  st: usize, //札の再生開始位置
  len: usize, //札の再生の長さ
}
impl Data {
  const fn new() -> Self {
    Self { idx:0, pos:0, st:0, len:0 }
  }
}
pub struct RndInfo {
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


const fn weight(x: i32) -> i64 {
  //(x as i64) * (x as i64)
  x.abs() as i64
}

// 問題の数列から数字を引いたやつのスコアを計算する
fn calc_score(a: &Box<[ValType; ANS_LEN]>) -> i64 {
  let mut score: i64 = 0;
  for i in 0..ANS_LEN {
    score += weight(a[i]);
  }
  return score;
}


fn read_answer(answer: &mut [Data; M]){
  let mut contents = String::new();
  stdin().read_line(&mut contents).expect("could not read answer idx");
  let v = read_vec(&contents);
  for i in 0..M {
    answer[i].idx = v[i] as usize;
  }
}

pub struct Solver {
  //problem: [i32; ANS_LEN],
  problem_length: usize,
  arrays: Box<[[i32; ANALYZE_MAX_LEN]; N]>,
  audio_length: [usize; N],

  best: [Data; M],
  used_idx: [bool; HALF_N],
  best_sub: Box<[ValType; ANS_LEN]>,
  pub best_score: i64,
  answer: [Data; M],
  rng: ThreadRng,
}
impl Solver {
  pub fn new() -> Self {
    //let mut problem = [0; ANS_LEN];
    let mut best_sub = Box::new([0; ANS_LEN]);
    let problem_length = read_problem_audio(&mut best_sub);
    let mut arrays = Box::new([[0; ANALYZE_MAX_LEN]; N]);
    let audio_length = read_audio_arrays(&mut arrays);
    let mut answer = [Data::new(); M];
    read_answer(&mut answer);
    
    let mut best: [Data; M] = [Data::new(); M];
    let mut used_idx = [false; HALF_N];
    for i in 0..M {
      best[i].idx = i;
      best[i].len = min(problem_length, audio_length[best[i].idx]);
      used_idx[i] = true;
      for j in 0..best[i].len {
        best_sub[j + best[i].pos] -= arrays[best[i].idx][j + best[i].st];
      }
    }
    let best_score = calc_score(&best_sub);
    eprintln!("First Score: {}", best_score);
    Self {
      //problem,
      problem_length, arrays, audio_length,
      best, used_idx, best_sub, best_score,
      answer,
      rng: thread_rng(),
    }
  }

  pub fn rnd_create(&mut self, change: &mut RndInfo){
    let t = self.rng.gen_range(0..10) >= 2;
    if t == false {
      change.idx = self.rng.gen_range(0..M);
      change.nxt_idx = self.best[change.idx].idx;
      change.len = self.rng.gen_range(HZ..=min(self.problem_length, self.audio_length[change.nxt_idx]));
      change.st = self.rng.gen_range(0..=self.audio_length[change.nxt_idx]-change.len);
      change.pos = self.rng.gen_range(0..=self.problem_length-change.len);
    }
    else{
      change.idx = self.rng.gen_range(0..M);
      change.nxt_idx = self.rng.gen_range(0..N);
      while self.used_idx[change.nxt_idx % HALF_N] && self.best[change.idx].idx % HALF_N != change.nxt_idx % HALF_N {
        change.nxt_idx = self.rng.gen_range(0..N);
      }
      change.len = self.rng.gen_range(HZ..=min(self.problem_length, self.audio_length[change.nxt_idx]));
      change.st = self.rng.gen_range(0..=self.audio_length[change.nxt_idx]-change.len);
      change.pos = self.rng.gen_range(0..=self.problem_length-change.len);
    }
  }
  pub fn rnd_create2(&mut self) -> RndInfo {
    let mut change = RndInfo::new();
    self.rnd_create(&mut change);
    return change;
  }
  pub fn calc_one_changed_ans(&self, info: &RndInfo) -> i64 {
    let pre: &Data = &self.best[info.idx];
    let info_rig = info.pos + info.len;
    let pre_rig = pre.pos + pre.len;
    let mut score = self.best_score;
    if max(info.pos, pre.pos) < min(info_rig, pre_rig) {
      // 左側がleft
      if info.pos <= pre.pos {
        let rightest = min(info_rig, pre.pos);
        assert!(rightest >= info.pos);
        for i in 0..rightest-info.pos {
          score += weight(self.best_sub[i + info.pos] - self.arrays[info.nxt_idx][i + info.st]) - weight(self.best_sub[i + info.pos]);
        }
      }
      // 左側がright
      else{
        let rightest = min(pre_rig, info.pos);
        for i in 0..rightest-pre.pos {
          score += weight(self.best_sub[i + pre.pos] + self.arrays[pre.idx][i + pre.st]) - weight(self.best_sub[i + pre.pos]);
        }
      }
      // 右側がright
      if info_rig <= pre_rig {
        let leftest = max(pre.pos, info_rig);
        let s = if info_rig > pre.pos { info_rig - pre.pos } else { 0 };
        for i in 0..pre_rig-leftest {
          score += weight(self.best_sub[i + leftest] + self.arrays[pre.idx][i + s + pre.st]) - weight(self.best_sub[i + leftest]);
        }
      }
      // 右側がleft
      else{
        let leftest = max(info.pos, pre_rig);
        let s = if pre_rig > info.pos { pre_rig - info.pos } else { 0 };
        for i in 0..info_rig-leftest {
          score += weight(self.best_sub[i + leftest] - self.arrays[info.nxt_idx][i + s + info.st]) - weight(self.best_sub[i + leftest]);
        }
      }
      // middle
      let leftest = max(info.pos, pre.pos);
      let range = min(info_rig, pre_rig) - leftest;
      let sl = if pre.pos > info.pos { pre.pos - info.pos } else { 0 };
      let sr = if info.pos > pre.pos { info.pos - pre.pos } else { 0 };
      for i in 0..range {
        score -= weight(self.best_sub[i + leftest]);
        let d: ValType = self.best_sub[i + leftest] - self.arrays[info.nxt_idx][i + sl + info.st] + self.arrays[pre.idx][i + sr + pre.st];
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
  pub fn update_values(&mut self, info: &RndInfo){
    let pre: &Data = &self.best[info.idx];
    for i in 0..pre.len {
      self.best_sub[i + pre.pos] += self.arrays[pre.idx][i + pre.st];
    }
    for i in 0..info.len {
      self.best_sub[i + info.pos] -= self.arrays[info.nxt_idx][i + info.st];
    }
    // update info
    self.used_idx[self.best[info.idx].idx % HALF_N] = false;
    self.used_idx[info.nxt_idx % HALF_N] = true;
    self.best[info.idx].idx = info.nxt_idx;
    self.best[info.idx].pos = info.pos;
    self.best[info.idx].st = info.st;
    self.best[info.idx].len = info.len;
  }
  pub fn output_result(&self){
    println!("answer");
    for i in 0..M {
      if self.answer[i].idx < HALF_N {
        print!("J{} ", self.answer[i].idx + 1);
      }else{
        print!("E{} ", self.answer[i].idx - HALF_N + 1);
      }
    }
    println!();
    println!("result");
    for i in 0..M {
      if self.best[i].idx < HALF_N {
        print!("J{} ", self.best[i].idx + 1);
      }else{
        print!("E{} ", self.best[i].idx - HALF_N + 1);
      }
    }
    // if has_answer
    println!();
    let mut diff_num = 0;
    for i in 0..M {
      let mut ok = false;
      for j in 0..M {
        if self.best[i].idx == self.answer[j].idx {
          ok = true; break;
        }
      }
      if ok { continue; }
      diff_num += 1;
      if self.best[i].idx < HALF_N {
        print!("J{} ", self.best[i].idx + 1);
      }else{
        print!("E{} ", self.best[i].idx - HALF_N + 1);
      }
    }
    eprintln!("Diff: {}/{}", diff_num, M);
  }

}

