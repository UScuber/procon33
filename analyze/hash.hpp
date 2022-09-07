#include <vector>
#include <string>
#include <chrono>
#include <bitset>

using ull = unsigned long long;

template <class Key, int logn>
struct HashSet {
  using uint = unsigned int;
  private:
  static constexpr unsigned int N = 1 << logn;
  Key *keys;
  std::bitset<N> flag;
  ull r;
  static constexpr uint shift = 64 - logn;
  ull rng() const{
    ull m = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    m ^= m >> 16;
    m ^= m << 32;
    return m;
  }
  public:
  HashSet() : keys(new Key[N]), r(rng()){}
  ~HashSet(){
    delete keys;
  }
  void set(const Key &i){
    uint hash = (ull(i) * r) >> shift;
    while(true){
      if(!flag[hash]){
        keys[hash] = i;
        flag[hash] = 1;
        return;
      }
      if(keys[hash] == i) return;
      hash = (hash + 1) & (N - 1);
    }
  }
  bool find(const Key &i){
    uint hash = (ull(i) * r) >> shift;
    while(true){
      if(!flag[hash]) return false;
      if(keys[hash] == i) return true;
      hash = (hash + 1) & (N - 1);
    }
  }
};

template <int max_len>
struct Hash {
  static constexpr ull m = (1ULL << 61) - 1;
  const ull base;
  std::vector<ull> h;
  Hash(const ull base, const ull power[]) : base(base), power(power){}
  inline ull query(int l, int r) const{
    assert(max_len >= r - l);
    assert(0 <= l && l <= r && r < h.size());
    return add(h[r], m - mul(h[l], power[r - l]));
  }
  void combine(const Hash<max_len> &a){
    const int len = h.size();
    h.insert(h.end(), a.h.begin()+1, a.h.end());
    const int tot_len = h.size();
    ull val = h[len - 1];
    for(int i = len; i < tot_len; i++){
      val = mul(val, base);
      h[i] = add(val, h[i]);
    }
  }
  inline int size() const{ return h.size(); }
  private:
  const ull *power;
  inline ull add(ull a, const ull b) const{
    if((a += b) >= m) a -= m;
    return a;
  }
  inline ull mul(const ull a, const ull b) const{
    const __uint128_t c = (__uint128_t)a * b;
    return add(c >> 61, c & m);
  }
  inline ull fma(const ull a, const ull b, const ull c) const{
    const __uint128_t d = (__uint128_t)a * b + c;
    return add(d >> 61, d & m);
  }
};

template <int max_len>
struct RollingHash {
  static constexpr ull m = (1ULL << 61) - 1;
  const ull base;
  RollingHash() : base(rnd()){
    power[0] = 1;
    for(int i = 0; i < max_len; i++){
      power[i + 1] = mul(power[i], base);
    }
  }
  Hash<max_len> gen(const std::string &s) const{
    const int len = s.size();
    Hash<max_len> hash(base, power);
    hash.h.resize(len + 1);
    for(int i = 0; i < len; i++){
      hash.h[i+1] = fma(hash.h[i], base, s[i]);
    }
    return hash;
  }
  template <class T>
  Hash<max_len> gen(const std::vector<T> &s) const{
    const int len = s.size();
    Hash<max_len> hash(base, power);
    hash.h.resize(len + 1);
    for(int i = 0; i < len; i++){
      hash.h[i+1] = fma(hash.h[i], base, s[i]);
    }
    return hash;
  }
  ull combine(ull h1, ull h2, ull h2_len) const{
    assert(max_len >= h2_len);
    return fma(h1, power[h2_len], h2);
  }
  ull combine(const Hash<max_len> &a, int l1, int r1, const Hash<max_len> &b, int l2, int r2) const{
    assert(max_len >= r2 - l2);
    return fma(a.query(l1, r1), power[r2-l2], b.query(l2, r2));
  }
  private:
  ull power[max_len + 1];
  ull rnd() const{
    ull b = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    b ^= b >> 16;
    b ^= b << 32;
    return b % (m - 2) + 2;
  }
  inline ull add(ull a, const ull b) const{
    if((a += b) >= m) a -= m;
    return a;
  }
  inline ull mul(const ull a, const ull b) const{
    const __uint128_t c = (__uint128_t)a * b;
    return add(c >> 61, c & m);
  }
  inline ull fma(const ull a, const ull b, const ull c) const{
    const __uint128_t d = (__uint128_t)a * b + c;
    return add(d >> 61, d & m);
  }
};
