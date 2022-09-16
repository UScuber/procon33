from pydub import AudioSegment
import glob

file_names = glob.glob("audio/*.wav")

sound = 0

for file_name in file_names:
    sound_tmp = AudioSegment.from_file(file_name, "wav")
    sound += sound_tmp

sound.export("output.wav", format="wav")