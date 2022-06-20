use std::io::*;

pub const N: usize = 44*2;
pub const M: usize = 20;
pub const INF: i32 = i32::MAX;
pub const DEFAULT_SAMP_HZ: usize = 48000;
pub const ANALYZE_SAMP_HZ: usize = 48000;
pub const ANALYZE_MAX_LEN: usize = 394606 / (DEFAULT_SAMP_HZ / ANALYZE_SAMP_HZ);
pub const ANS_LEN: usize = ANALYZE_SAMP_HZ * 8;

/*
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
*/
pub fn read_int(file: &mut String) -> i32 {
  let token: String = file
    .bytes()
    .map(|c| c as char)
    .skip_while(|c| c.is_whitespace())
    .take_while(|c| !c.is_whitespace())
    .collect();
  return token.parse().ok().expect("failed to parse token");
}

pub fn read_problem_audio(problem: &mut Box<[i32; ANS_LEN]>) -> usize {
  let mut contents = String::new();
  stdin().read_line(&mut contents).expect("could not read problem audio");
  let mut problem_len = ANS_LEN;
  for i in 0..ANS_LEN {
    problem[i] = read_int(&mut contents);
    if problem[i] == INF {
      problem[i] = 0;
      problem_len = i;
      break;
    }
  }
  return problem_len;
}
pub fn read_audio_arrays(arrays: &mut Box<[[i32; ANALYZE_MAX_LEN]; N]>) -> [usize; N] {
  let mut array_len = [ANALYZE_MAX_LEN; N];
  for i in 0..N {
    let mut contents = String::new();
    stdin().read_line(&mut contents).expect("could not read audio arrays");
    for j in 0..ANALYZE_MAX_LEN {
      arrays[i][j] = read_int(&mut contents);
      if arrays[i][j] == INF {
        arrays[i][j] = 0;
        array_len[i] = j;
        break;
      }
    }
  }
  return array_len;
}
