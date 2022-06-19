use rand::prelude::*;
mod solver;
mod file_reader;

fn main(){
  let mut rnd = thread_rng();
  let mut a = [0; 5];
  for i in 0..5 {
    a[i] = rnd.gen_range(0..5);
  }
  for i in 0..5 {
    println!("{:05}", a[i]);
  }
}
