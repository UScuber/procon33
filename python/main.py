import numpy as np
from scipy import signal
import os
import soundfile as sf
import pyroomacoustics as pra

path_to_pyroom = 'audio/JKspeech'
outputDir = './output'

fsResample = 16000  # resampling frequency [Hz]
fftSize = 4096  # window length in STFT [points]
shiftSize = 2048  # shift length in STFT [points]
ns = 2  # number of sources
it = 100  # number of iterations
nb = 10  # number of bases

os.makedirs(outputDir, exist_ok=True)

# signal x channel x source (source image)
sig_src1, fs = sf.read(path_to_pyroom + '/J01.wav')
sig_src2, fs = sf.read(path_to_pyroom + '/J02.wav')
for i in range(min(len(sig_src1), len(sig_src2))):
  sig_src1[i] += sig_src2[i]
del sig_src2
sig_src2, fs = sf.read(path_to_pyroom + '/E01.wav')

min_len = min(len(sig_src1), len(sig_src2))
sig_src1 = sig_src1[:min_len]
sig_src2 = sig_src2[:min_len]
sig = np.stack([sig_src1, sig_src2], axis=1)
del sig_src1, sig_src2

sig_src1 = signal.resample_poly(sig[:, 0], fsResample, fs)
sig_src2 = signal.resample_poly(sig[:, 1], fsResample, fs)
sig_resample = np.stack([sig_src1, sig_src2], axis=1)
del sig_src1, sig_src2

#save first
# source signal 1
sf.write('{}/originalSource1.wav'.format(outputDir), sig_resample[:, 0], fsResample)
# source signal 2
sf.write('{}/originalSource2.wav'.format(outputDir), sig_resample[:, 1], fsResample)

mix1 = sig_resample[:, 0]
mix1 += sig_resample[:, 1]
mix2 = sig_resample[:, 1]
mix2 += sig_resample[:, 0]
mix = np.stack([mix1, mix2], axis=1)
del mix1, mix2
# analysis window
win_a = pra.hamming(fftSize)

# optimal synthesis window
win_s = pra.transform.compute_synthesis_window(win_a, shiftSize)

# STFT
X = pra.transform.analysis(mix, fftSize, shiftSize, win=win_a)

# Apply FastMNMF
Y = pra.bss.fastmnmf(X, n_src=ns, n_iter=it, n_components=nb)

# ISTFT
y = pra.transform.synthesis(Y, fftSize, shiftSize, win=win_s)


# observed signal
sf.write('{}/observedMixture.wav'.format(outputDir), mix, fsResample)

print("y:", y)

# estimated signal 1
sf.write('{}/estimatedSignal1.wav'.format(outputDir), y[:, 0], fsResample)

# estimated signal 2
sf.write('{}/estimatedSignal2.wav'.format(outputDir), y[:, 1], fsResample)


print('The files are saved in "./output".')