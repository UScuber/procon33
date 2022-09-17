from pydub import AudioSegment
import glob
import os

def wav_cat():
    # 現在のディレクトリ
    cur_dir = os.getcwd()

    # audioディレクトリに入っているwavファイル名(パス)をリスト形式で取得
    file_names = glob.glob("audio/*.wav")

    sound = 0

    # 取得したwavファイル名(パス)を順に結合していく
    for file_name in file_names:
        sound_tmp = AudioSegment.from_file(file_name, "wav")
        sound += sound_tmp

    # analyze/testディレクトリの方に移って結合したwavファイルを出力する
    os.chdir("../analyze/test")
    sound.export("problem.wav", format="wav")

    # 現在のディレクトリに移って受け取った分割データを全削除
    os.chdir(cur_dir)
    for file_name in file_names:
        os.remove(file_name)

if __name__ == "__main__":
    wav_cat()