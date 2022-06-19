use std::io::*;
use std::fs;
//static arrays: Vec<Vec<i32>> = Lazy::new(|| read_array());

pub const N: usize = 44*2;
pub const M: usize = 20;
pub const INF: i32 = i32::MAX;
pub const INFL: i64 = i64::MAX;
pub const DEFAULT_SAMP_HZ: usize = 48000;
pub const ANALYZE_SAMP_HZ: usize = 48000;
pub const ANALYZE_MAX_LEN: usize = 394606 / (DEFAULT_SAMP_HZ / ANALYZE_SAMP_HZ);
pub const ANS_LEN: usize = ANALYZE_SAMP_HZ * 8;

fn read<T: std::str::FromStr>() -> T {
  let stdin = stdin();
  let stdin = stdin.lock();
  let token: String = stdin
    .bytes()
    .map(|c| c.expect("failed to read char") as char)
    .skip_while(|c| c.is_whitespace())
    .take_while(|c| !c.is_whitespace())
    .collect();
  token.parse().ok().expect("failed to parse token")
}
fn read_int(file: &mut String) -> i32 {
  let token: String = file
    .bytes()
    .map(|c| c as char)
    .skip_while(|c| c.is_whitespace())
    .take_while(|c| !c.is_whitespace())
    .collect();
  return token.parse().ok().expect("failed to parse token");
}

pub fn read_audio_arrays() -> [[i32; ANALYZE_MAX_LEN]; N] {
  let mut content: String = fs::read_to_string("audio_arrays.txt").expect("could not read audio_arrays.txt");
  let mut audios = [[0; ANALYZE_MAX_LEN]; N];
  for i in 0..N {
    for j in 0..ANALYZE_MAX_LEN {
      //audios[i][j] = read();
      audios[i][j] = read_int(&mut content);
      if audios[i][j] == INF {
        audios[i][j] = 0;
        break;
      }
    }
  }
  return audios;
}
pub fn read_problem_audio() -> [i32; ANALYZE_MAX_LEN] {
  let mut content: String = fs::read_to_string("problem.txt").expect("could not read problem.txt");
  let mut audio = [0; ANALYZE_MAX_LEN];
  for i in 0..ANS_LEN {
    //audio[i] = read();
    audio[i] = read_int(&mut content);
    if audio[i] == INF {
      audio[i] = 0;
      break;
    }
  }
  return audio;
}
pub fn read_answer_idx() -> [i32; M] {
  let mut answer: [i32; M] = [0; M];
  for i in 0..M {
    answer[i] = read();
  }
  return answer;
}

pub fn read_audio_length() -> [usize; N] {
  let mut length = [0 as usize; N];
  let mut content: String = fs::read_to_string("audio_length.txt").except("could not read audio_length.txt");
  for i in 0..N {
    length[i] = read_int(&mut content);
  }
  return length;
}
pub fn read_problem_length() -> usize {
  let mut content: String = fs::read_to_string("problem_length.txt").except("could not read problem_length.txt");
  return read_int(&mut content);
}