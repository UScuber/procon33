#include "form.hpp"


WaveForm::WaveForm(Audio &&audio) : Audio(audio){}
void WaveForm::setSize(int w, int h){ width = w; height = h; }
void WaveForm::setPosition(const Vec2 &pos){ dpos = pos; }
void WaveForm::setPosition(int x, int y){ dpos = { x, y }; } 

void WaveForm::update(const Font &font, bool force_update){
	if(force_update || this->isPlaying()) FFT::Analyze(fft, *this);

	// 周波数の表示
	for(int i = 0; i < Min(width, (int)fft.buffer.size()); i++){
		const double size = Pow(fft.buffer[i], 0.6f) * 1000;
		RectF(Arg::bottomLeft(i + dpos.x, height*0.96 + dpos.y), 1, size).draw(Color(0, 100, 255));
	}
	// 目盛りの描画
	// font-size は 10の時480がベスト
	for(int i = 0; i < width; i += width/20){
		font(U"{}"_fmt((int)(fft.resolution * i))).draw(Arg::topCenter(i + dpos.x, height*0.96 + dpos.y));
	}
}


void WaveTextReader::import_wave(const String &file_name){
	reader = TextReader(U"../music/" + file_name + U".txt");
	if(!reader.isOpen()){
		Console << U"The file was not found";
		return;
	}
	length = Parse<int>(reader.readLine().value());
	data.resize(length);
	resolution = Parse<double>(reader.readLine().value());
	for(int i = 0; i < length; i++){
		for(const auto &x : reader.readLine().value().split(' ')){
			if(x.isEmpty()) continue;
			data[i].push_back(Parse<int>(x));
		}
		assert(data[i].size() == 600);
		for(int j = 0; j < 600; j++){
			Rect(Arg::bottomLeft(j, 480), 1, data[i][j]).draw();
		}
	}
}
void WaveTextReader::display(const int &frame){
	assert(0 <= frame && frame < length);
	// 周波数の表示
	for(int i = 0; i < 600; i++){
		const int size = data[frame][i];
		RectF(Arg::bottomLeft(i + 10, 480), 1, size).draw(Color(0, 100, 255));
	}
}


void export_wave(const String &file_name, const int &fps){
	Audio audio = Dialog::OpenAudio();
	TextWriter writer(U"../music/" + file_name + U".txt");
	const int length = int(audio.lengthSec() * fps);
	writer.writeln(length);
	FFTResult fft;
	FFT::Analyze(fft, audio);
	writer.writeln(fft.resolution);
	for(int i = 0; i < length; i++){
		audio.seekTime((double)i / fps);
		FFT::Analyze(fft, audio);
		for(int j = 0; j < 600; j++){
			//const double size = Pow(fft.buffer[j], 0.6) * 1000;
			const double size = fft.buffer[j] * 3000 * 4;
			writer.write((int)size);
			writer.write(U" ");
		}
		writer.writeln(U"");
	}
}
