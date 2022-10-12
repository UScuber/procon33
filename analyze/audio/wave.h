#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define short_max __INT16_MAX__
#define default_sampling_hz 48000
#define analyze_change_prop 3
#ifdef USE_MULTI_THREAD
#define analyze_sampling_hz 6000
#else
#define analyze_sampling_hz 2000
#endif
#define default_audio_max_length 394606
#define analyze_audio_max_length (default_audio_max_length / (default_sampling_hz / analyze_sampling_hz))
//static_assert(default_sampling_hz % analyze_sampling_hz == 0);

typedef struct {
  int fs; //サンプリング周波数
  int bits; //量子化bit数
  int L; //データ長
  int *data; //データ
} Wave;

void read_audio(Wave *prm, const char *filename){
  char tmp[24];

  int fmt_samples_per_sec;
  short fmt_bits_per_sample;
  int data_size;

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
  prm->fs = fmt_samples_per_sec;
  prm->bits = fmt_bits_per_sample;
  prm->L = data_size / 2;

  //音声データ代入
  if(prm->data != NULL) prm->data = (int*)realloc(prm->data, prm->L * sizeof(int));
  else prm->data = (int*)malloc(prm->L * sizeof(int));
  for(int i = 0; i < prm->L; i++){
    short data_data;
    fread(&data_data, 2, 1, fp);
    prm->data[i] = data_data;
  }
  fclose(fp);
}
void write_audio(const Wave *prm, const char *filename){
  FILE *fp = fopen(filename, "wb");
  assert(fp != NULL);

  //ヘッダー書き込み
  const int header_size = 36 + prm->L * 2;

  fwrite("RIFF", 1, 4, fp);
  fwrite(&header_size, 4, 1, fp);
  fwrite("WAVE", 1, 4, fp);
  fwrite("fmt ", 1, 4, fp);

  const int fmt_size = 16;
  const short fmt_format = 1;
  const short fmt_channel = 1;
  const int fmt_samples_per_sec = prm->fs;
  const int fmt_bytes_per_sec = prm->fs * prm->bits / 8;
  const short fmt_block_size = prm->bits / 8;
  const short fmt_bits_per_sample = prm->bits;
  fwrite(&fmt_size, 4, 1, fp);
  fwrite(&fmt_format, 2, 1, fp);
  fwrite(&fmt_channel, 2, 1, fp);
  fwrite(&fmt_samples_per_sec, 4, 1, fp);
  fwrite(&fmt_bytes_per_sec, 4, 1, fp);
  fwrite(&fmt_block_size, 2, 1, fp);
  fwrite(&fmt_bits_per_sample, 2, 1, fp);

  //データ書き込み
  const int data_size = prm->L * 2;
  fwrite("data", 1, 4, fp);
  fwrite(&data_size, 4, 1, fp);

  //音声データ書き込み
  for(int i = 0; i < prm->L; i++){
    short data_data;
    //リミッター
    if(prm->data[i] > short_max) data_data = short_max;
    else if(prm->data[i] < -short_max) data_data = -short_max;
    else data_data = prm->data[i];
    fwrite(&data_data, 2, 1, fp);
  }
  fclose(fp);
}


//prm.fs[Hz] -> fs[Hz]
void change_sampling_hz(Wave *prm, int fs){
  assert(prm->fs % fs == 0);
  const int p = prm->fs / fs;
  prm->fs = fs;
  for(int i = 0; i < prm->L / p; i++){
    prm->data[i] = prm->data[i * p];
  }
  prm->L /= p;
}
