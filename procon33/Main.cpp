#include <Siv3D.hpp> // OpenSiv3D v0.6.3

#include "analyze.hpp"


void Main(){
	// 音楽
	Audio audio;
	// FFT の結果
	FFTResult fft;
	// 再生位置の変更の有無
	bool hasChanged = false;
	while(System::Update()){
		// 再生・演奏時間
		const String time = FormatTime(SecondsF(audio.posSec()), U"M:ss")
			+ U'/' + FormatTime(SecondsF(audio.lengthSec()), U"M:ss");
		// プログレスバーの進み具合
		double progress = static_cast<double>(audio.posSample()) / audio.samples();

		// FFT 解析
		FFT::Analyze(fft, audio);
		// 結果を可視化
		for(int i = 0; i < 800; i++){
			const double size = Pow(fft.buffer[i], 0.6f) * 1000;
			RectF(Arg::bottomLeft(i, 480), 1, size).draw(HSV(240 - i));
		}
		// 周波数表示
		Rect(Cursor::Pos().x, 0, 1, Scene::Height()).draw();
		ClearPrint();
		Print << U"{} Hz"_fmt(Cursor::Pos().x * fft.resolution);

		// Start, Pause Button
		if(SimpleGUI::Button(U"Play/Pause", Vec2(40, 500), 120, (bool)audio)){
			if(!audio.isPlaying()){
				audio.play();
			}else{
				audio.pause();
			}
		}

		// フォルダから音楽ファイルを開く
		if(SimpleGUI::Button(U"Open", Vec2(300, 500), 120)){
			audio.stop(0.5s);
			audio = Dialog::OpenAudio();
			audio.play();
		}

		// スライダー
		if(SimpleGUI::Slider(time, progress, Vec2(40, 540), 130, 590, !audio.isEmpty())){
			audio.pause(0.1s);
			// 再生位置を変更
			audio.seekSamples(static_cast<int64>(audio.samples() * progress));
			// ノイズを避けるため、スライダーから手を離すまで再生は再開しない
			hasChanged = true;
		}
		else if(hasChanged && MouseL.up()){
			// 再生を再開
			audio.play(0.1s);
			hasChanged = false;
		}
	}
}
