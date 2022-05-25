#include <bits/stdc++.h>
#define rep(i, n) for(int i = 0; i < (n); i++)
using std::vector;

//URL: https://hwswsgps.hatenablog.com/entry/2018/08/19/172401
struct WAV_PRM {
  int fs; //サンプリング周波数
  int bits; //量子化bit数
  int L; //データ長
};

vector<int> audio_read(WAV_PRM &prm, char *filename){
  char tmp[24];

  long fmt_samples_per_sec;
  short fmt_bits_per_sample;
  long data_size;

  //wavファイルオープン
  FILE *fp = fopen(filename, "rb");
  assert(fp != NULL);

  //wavデータ読み込み
  fread(tmp, 1, 24, fp);
  fread(&fmt_samples_per_sec, 4, 1, fp);
  fread(tmp, 1, 6, fp);
  fread(&fmt_bits_per_sample, 2, 1, fp);
  fread(tmp, 1, 4, fp);
  fread(&data_size, 4, 1, fp);

  //パラメータ代入
  prm.fs = fmt_samples_per_sec;
  prm.bits = fmt_bits_per_sample;
  prm.L = data_size / 2;

  //音声データ代入
  vector<int> data(prm.L);
  for(int i = 0; i < prm.L; i++){
    short data_data;
    fread(&data_data, 2, 1, fp);
    data[i] = data_data;
  }

  fclose(fp);
  return data;
}
void audio_write(const vector<int> &data, const WAV_PRM &prm, char *filename){
  FILE *fp = fopen(filename, "wb");
  assert(fp != NULL);

  //ヘッダー書き込み
  const long header_size = 36 + prm.L * 2;

  fwrite("RIFF", 1, 4, fp);
  fwrite(&header_size, 4, 1, fp);
  fwrite("WAVE", 1, 4, fp);
  fwrite("fmt ", 1, 4, fp);

  const long fmt_size = 16;
  const short fmt_format = 1;
  const short fmt_channel = 1;
  const long fmt_samples_per_sec = prm.fs;
  const long fmt_bytes_per_sec = prm.fs * prm.bits / 8;
  const short fmt_block_size = prm.bits / 8;
  const short fmt_bits_per_sample = prm.bits;
  fwrite(&fmt_size, 4, 1, fp);
  fwrite(&fmt_format, 2, 1, fp);
  fwrite(&fmt_channel, 2, 1, fp);
  fwrite(&fmt_samples_per_sec, 4, 1, fp);
  fwrite(&fmt_bytes_per_sec, 4, 1, fp);
  fwrite(&fmt_block_size, 2, 1, fp);
  fwrite(&fmt_bits_per_sample, 2, 1, fp);

  //データ書き込み
  const long data_size = prm.L * 2;
  fwrite("data", 1, 4, fp);
  fwrite(&data_size, 4, 1, fp);

  //音声データ書き込み
  for(int i = 0; i < prm.L; i++){
    short data_data;
    //リミッター
    if(data[i] > 32767){
      data_data = 32767;
    }else if(data[i] < -32767){
      data_data = -32767;
    }else{
      data_data = data[i];
    }
    fwrite(&data_data, 2, 1, fp);
  }
  fclose(fp);
}

//prm.fs[Hz] -> fs[Hz]
vector<int> change_sampling_hz(const vector<int> &data, WAV_PRM &prm, int fs){
  assert(prm.fs % fs == 0);
  const int p = prm.fs / fs;
  prm.fs = fs;
  prm.L /= p;
  vector<int> new_data(prm.L);
  rep(i, prm.L){
    int tot = 0;
    //rep(j, p) tot += data[i*p + j];
    //new_data[i] = tot / p;
    new_data[i] = data[i * p];
  }
  return new_data;
}


//a.exe [import file name] [export file name]
int main(int argc, char *args[]){
  WAV_PRM prm;
  vector<int> data = audio_read(prm, args[1]);
  printf("bits:%d, sampling hz:%d\n", prm.bits, prm.fs);
  vector<int> new_data = change_sampling_hz(data, prm, 12000);
  audio_write(new_data, prm, args[2]);
  printf("completed\n");
}