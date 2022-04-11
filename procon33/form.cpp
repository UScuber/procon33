#include "form.hpp"

namespace Disp {

WaveForm::WaveForm(Audio &&audio) : audio(audio){}
void WaveForm::setSize(int w, int h){ width = w; height = h; }
void WaveForm::setPosition(const Vec2 &pos){ position = pos; }
void WaveForm::setPosition(int x, int y){ position = { x, y }; } 

void WaveForm::update(const double &time, const Font &font){
	audio.seekTime(time);
	FFT::Analyze(fft, audio);

	for(int i = 0; i < 800; i++){
		const double size = Pow(fft.buffer[i], 0.6f) * 1000;
		RectF(Arg::bottomLeft(i, 480), 1, size).draw(Color(0, 100, 255));
	}
	for(int i = 0; i < 800; i += 40){
		font(U"{}"_fmt((int)(fft.resolution * i))).draw(i, 480);
	}
}

}; // namespace Display
