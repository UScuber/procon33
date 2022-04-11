#include "form.hpp"

namespace Disp {

//WaveForm::WaveForm(Audio &&audio) : audio(audio){}
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

}; // namespace Disp
