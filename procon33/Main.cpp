#include <Siv3D.hpp> // OpenSiv3D v0.6.3

#include "form.hpp"


void Main(){
	const Font font(10);
	// 再生位置の変更の有無
	bool hasChanged = false;
	bool is_stopping = false;
	bool time_changed = false;

	Disp::WaveForm audio(Dialog::OpenAudio());
	audio.setSize(800, 500);
	audio.setPosition(0, 0);
	audio.play();
	while(System::Update()){
		audio.update(font);

		ClearPrint();
		if(!is_stopping || time_changed){
			audio.update(font, true);
		}
		time_changed = false;

		// 再生・演奏時間
		const String time = FormatTime(SecondsF(audio.posSec()), U"M:ss")
			+ U'/' + FormatTime(SecondsF(audio.lengthSec()), U"M:ss");
		// プログレスバーの進み具合
		double progress = static_cast<double>(audio.posSample()) / audio.samples();


		// 周波数表示
		Rect(Cursor::Pos().x, 0, 1, Scene::Height()).draw();
		Print << U"{} Hz"_fmt(Cursor::Pos().x * audio.fft.resolution);
		double loud = -1, hz = -1;
		for(int i = 0; ; i++){
			const double size = Pow(audio.fft.buffer[i], 0.6f) * 1000;
			const double pos = i * audio.fft.resolution;
			if(loud < size){
				loud = size;
				hz = pos;
			}
			if(pos > 400.0) break;
		}
		if(hz > 0){
			double pos = hz / audio.fft.resolution;
			for(double freq = hz; pos < 800; freq += hz){
				pos = freq / audio.fft.resolution;
				RectF(Arg::bottomLeft(pos, 480), 1, Scene::Height()).draw(Palette::Gray);
			}
		}

		// Start, Pause Button
		const String button_name = audio.isPlaying() ? U"Pause" : U"Play";
		if(!hasChanged) is_stopping = !audio.isPlaying();
		if(SimpleGUI::Button(button_name, Vec2(40, 500), 120, (bool)audio) || (audio && KeySpace.down())){
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
		}

		// slider sample time
		if(KeyComma.down()){
			audio.seekTime(audio.posSec() - 1.0/30);
			time_changed = true;
		}
		if(KeyPeriod.down()){
			audio.seekTime(audio.posSec() + 1.0/30);
			time_changed = true;
		}
		// slider 3s
		if(KeyLeft.down()){
			audio.seekTime(audio.posSec() - 1);
			time_changed = true;
		}
		if(KeyRight.down()){
			audio.seekTime(audio.posSec() + 1);
			time_changed = true;
		}
		// スライダー
		if(SimpleGUI::Slider(time, progress, Vec2(40, 540), 130, 590, !audio.isEmpty())){
			audio.pause(0.5s);
			// 再生位置を変更
			audio.seekSamples(static_cast<int64>(audio.samples() * progress));
			// ノイズを避けるため、スライダーから手を離すまで再生は再開しない
			hasChanged = true;
		}
		else if(hasChanged && MouseL.up()){
			// 再生を再開
			if(!is_stopping) audio.play();
			hasChanged = false;
		}
	}
	/*
	Audio audio = Dialog::OpenAudio();
	TextWriter writer(U"../music/array11.txt");
	FFTResult fft;
	for(int i = 0; i < 5*30; i++){
		audio.seekTime(i / 30.0);
		FFT::Analyze(fft, audio);
		for(int j = 0; j < 600; j++){
			const double size = Pow(fft.buffer[j], 0.6) * 1000 / 4;
			writer.write((int)size);
			writer.write(U" ");
		}
		writer.writeln(U"");
	}
	*/
}
