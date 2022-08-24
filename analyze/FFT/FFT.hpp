#include <vector>
#include <cmath>
#include <cstring>
#include <cassert>
#pragma GCC optimize("unroll-loops")

#include "pffft.h"
#include "Memory.hpp"

constexpr int sampleLength = 5-3; // 8192 samples
constexpr int sampleRange = 256 << sampleLength;
constexpr int resolution = 128 << sampleLength;

struct FFT {
  FFT() : setup(pffft_new_setup(sampleRange, PFFFT_REAL)),
          m_inBuffer(AlignedMalloc<float, 16>(1 << 19)),
          m_outBuffer(AlignedMalloc<float, 16>(sampleRange)),
          m_workBuffer(AlignedMalloc<float, 16>(sampleRange)),
          length(0){}
  ~FFT(){
    AlignedFree(m_workBuffer);
    AlignedFree(m_inBuffer);
    AlignedFree(m_outBuffer);
    pffft_destroy_setup(setup);
  }
  constexpr inline void set_audio(const int wave[], const int size){
    length = size;
    for(int i = 0; i < length; i++){
      m_inBuffer[i] = wave[i] / 32768.0;
    }
  }
  inline void set_audio(const std::vector<int> &wave){
    length = (int)wave.size();
    for(int i = 0; i < length; i++){
      m_inBuffer[i] = wave[i] / 32768.0;
    }
  }
  constexpr inline void set_audio(const float wave[], const int size){
    length = size;
    std::memcpy(m_inBuffer, wave, size * sizeof(float));
  }
  constexpr inline void set_sample(const int pos, const float x) noexcept{
    assert(0 <= pos && pos < (1 << 19));
    if(pos+1 > length) length = pos+1;
    m_inBuffer[pos] = x;
  }

  // defined library.hpp
  inline float func(const float x) const noexcept;
  
  // do fast fourier transform [from, from + sampleRange)
  inline void doFFT(float result[], const int from){
    assert(from + sampleRange <= length);
    pffft_transform_ordered(setup, m_inBuffer + from, m_outBuffer, m_workBuffer, PFFFT_FORWARD);
    constexpr float m = 1.0 / resolution;
    const float *pSrc = m_outBuffer;

    for(int i = 0; i < resolution; i++){
      const float f0 = *pSrc++;
      const float f1 = *pSrc++;
      result[i] = std::sqrt(f0 * f0 + f1 * f1) * m;
      result[i] = func(result[i]);
    }
  }
  inline void doFFT(std::vector<float> &result, const int from){
    assert(from + sampleRange <= length);
    doFFT(result.data(), from);
  }
private:
  PFFFT_Setup *setup;
  float *m_inBuffer, *m_outBuffer, *m_workBuffer;
  int length;
};
