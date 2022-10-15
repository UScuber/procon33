#include <iostream>
#include <string>

int main(){
  std::string s;
  std::getline(std::cin, s);
  uint64_t used_idx = 0;
  int val = -1;
  for(int i = 0; i < (int)s.size(); i++){
    if(s[i] < '0' || '9' < s[i]){
      val = -1;
      continue;
    }
    if(val == -1) val = s[i] - '0';
    else{
      val = val * 10 + s[i] - '0';
      val--;
      used_idx |= 1ULL << val;
      val = -1;
    }
  }
  std::cout << used_idx << "\n";
}