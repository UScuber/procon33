#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cassert>

constexpr int short_max = (1 << 15) - 1;
constexpr int default_sampling_hz = 48000;
constexpr int analyze_sampling_hz = 6000;
constexpr int default_audio_max_length = 394606;
constexpr int analyze_audio_max_length = default_audio_max_length / (default_sampling_hz / analyze_sampling_hz);
static_assert(default_sampling_hz % analyze_sampling_hz == 0);

//URL: https://hwswsgps.hatenablog.com/entry/2018/08/19/172401
struct Wave {
  int fs; //サンプリング周波数
  int bits; //量子化bit数
  int L = 0; //データ長
  std::vector<int> data;
  int &operator[](const int i) noexcept{
    assert(0 <= i && i < L);
    return data[i];
  }
  const int operator[](const int i) const noexcept{
    assert(0 <= i && i < L);
    return data[i];
  }
};

template <typename T>
inline void file_read(T *dst, const size_t size, FILE *fp) noexcept{
  const size_t d = fread(dst, sizeof(T), size, fp);
  assert(d > 0);
}
template <typename T>
inline void file_write(const T *str, const size_t size, FILE *fp) noexcept{
  const size_t d = fwrite(str, sizeof(T), size, fp);
  assert(d > 0);
}

void read_audio(Wave &prm, const char *filename){
  char tmp[24];

  int fmt_samples_per_sec;
  short fmt_bits_per_sample;
  int data_size;

  //wavファイルオープン
  FILE *fp = fopen(filename, "rb");
  assert(fp != NULL);

  //wavデータ読み込み
  file_read(tmp, 24, fp);
  file_read(&fmt_samples_per_sec, 1, fp);
  file_read(tmp, 6, fp);
  file_read(&fmt_bits_per_sample, 1, fp);
  file_read(tmp, 4, fp);
  file_read(&data_size, 1, fp);

  //パラメータ代入
  prm.fs = fmt_samples_per_sec;
  prm.bits = fmt_bits_per_sample;
  prm.L = data_size / 2;

  //音声データ代入
  prm.data.resize(prm.L);
  for(int i = 0; i < prm.L; i++){
    short data_data;
    file_read(&data_data, 1, fp);
    prm[i] = data_data;
  }
  fclose(fp);
}
void write_audio(const Wave &prm, const char *filename){
  FILE *fp = fopen(filename, "wb");
  assert(fp != NULL);

  //ヘッダー書き込み
  const int header_size = 36 + prm.L * 2;

  file_write("RIFF", 4, fp);
  file_write(&header_size, 1, fp);
  file_write("WAVE", 4, fp);
  file_write("fmt ", 4, fp);

  const int fmt_size = 16;
  const short fmt_format = 1;
  const short fmt_channel = 1;
  const int fmt_samples_per_sec = prm.fs;
  const int fmt_bytes_per_sec = prm.fs * prm.bits / 8;
  const short fmt_block_size = prm.bits / 8;
  const short fmt_bits_per_sample = prm.bits;
  file_write(&fmt_size, 1, fp);
  file_write(&fmt_format, 1, fp);
  file_write(&fmt_channel, 1, fp);
  file_write(&fmt_samples_per_sec, 1, fp);
  file_write(&fmt_bytes_per_sec, 1, fp);
  file_write(&fmt_block_size, 1, fp);
  file_write(&fmt_bits_per_sample, 1, fp);

  //データ書き込み
  const int data_size = prm.L * 2;
  file_write("data", 4, fp);
  file_write(&data_size, 1, fp);

  //音声データ書き込み
  for(int i = 0; i < prm.L; i++){
    short data_data;
    //リミッター
    if(prm[i] > short_max) data_data = short_max;
    else if(prm[i] < -short_max) data_data = -short_max;
    else data_data = prm[i];
    fwrite(&data_data, 2, 1, fp);
  }
  fclose(fp);
}


//prm.fs[Hz] -> fs[Hz]
void change_sampling_hz(Wave &prm, int fs){
  assert(prm.fs % fs == 0);
  const int p = prm.fs / fs;
  prm.fs = fs;
  std::vector<int> new_data(prm.L / p);
  for(int i = 0; i < prm.L / p; i++){
    //int tot = 0;
    //for(int j = 0; j < p; j++) tot += prm[i*p + j];
    //new_data[i] = tot / p;
    new_data[i] = prm[i * p];
  }
  prm.L /= p;
  prm.data = new_data;
}

// st[i]: waves[i]の開始位置(1sample単位)
[[nodiscard]]
Wave merge_audio(const std::vector<Wave> &waves, const std::vector<int> &st){
  assert(waves.size() == st.size());
  Wave res;
  res.bits = waves[0].bits;
  res.fs = waves[0].fs;
  res.L = 0;
  const int n = waves.size();
  for(int i = 0; i < n; i++){
    assert(res.bits == waves[i].bits && res.fs == waves[i].fs);
    res.L = std::max(res.L, waves[i].L + st[i]);
  }
  res.data.assign(res.L, 0);
  for(int i = 0; i < n; i++){
    for(int j = 0; j < waves[i].L; j++){
      res[j + st[i]] += waves[i][j];
    }
  }
  for(int i = 0; i < res.L; i++){
    if(res[i] > short_max) res[i] = short_max;
    else if(res[i] < -short_max) res[i] = -short_max;
  }
  return res;
}

//sep[i]: 分割する位置。sep.front() = 0, sep.back() = wave.L
[[nodiscard]]
std::vector<Wave> separate_audio(const Wave &wave, const std::vector<int> &sep){
  const int n = sep.size();
  assert(sep[0] == 0 && sep[n - 1] == wave.L);
  for(int i = 1; i < n - 1; i++){
    assert(sep[i] < sep[i + 1]);
    assert(0 < sep[i] && sep[i] < wave.L);
  }
  std::vector<Wave> res(n - 1);
  for(int i = 0; i < n - 1; i++){
    res[i].bits = wave.bits;
    res[i].fs = wave.fs;
    res[i].L = sep[i + 1] - sep[i];
    for(int j = sep[i]; j < sep[i + 1]; j++){
      res[i].data.emplace_back(wave[j]);
    }
    assert(res[i].data.size() == sep[i + 1] - sep[i]);
  }
  return res;
}