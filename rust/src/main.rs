use std::{time::Instant};
mod solver;
mod file_reader;
use crate::solver::*;

const MASK: i32 = (1 << 7) - 1;
const TL: f64 = 60.0 * 1.0;

fn solve(){
  let mut solver = Solver::new();
  let mut update_num: i32 = 0;
  let mut last_upd_time: f64 = -1 as f64;
  let mut steps: i32 = 0;

  let mut spend_time: f64 = 0.0;
  let start_time = Instant::now();
  // 山登り法
  loop {
    if (steps & MASK) == 0 {
      spend_time = start_time.elapsed().as_millis() as f64;
      if spend_time > TL * 1000.0 {
        break;
      }
    }
    let change = solver.rnd_create2();
    let score = solver.calc_one_changed_ans(&change);
    if solver.best_score > score {
      solver.best_score = score;
      solver.update_values(&change);
      eprint!("u");
      update_num += 1;
      last_upd_time = spend_time;
    }
    steps += 1;
  }
  eprintln!();
  eprintln!("Steps: {}", steps);
  eprintln!("Updated: {}", update_num);
  eprintln!("Last Update: {}", last_upd_time/1000.0);
  eprintln!("Time per loop: {}", spend_time/1000.0/steps as f64);
  eprintln!("Final Score: {}", solver.best_score);
  
  solver.output_result();
}

fn main(){
  solve();
}
