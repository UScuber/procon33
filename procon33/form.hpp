#pragma once
#include <Siv3D.hpp> // OpenSiv3D v0.6.3


struct WaveForm : Audio {
	//Audio audio;
	FFTResult fft;
	int height = -1, width = -1;
	Vec2 dpos{ -1, -1 };

	WaveForm(Audio &&audio);

	// formのサイズを指定
	void setSize(int w, int h);
	// 左上の位置を指定
	void setPosition(const Vec2 &pos);
	// 左上の位置を指定
	void setPosition(int x, int y);
	// 
	void update(const Font &font, bool force_update = false);
};


struct WaveTextReader {
	TextReader reader;
	Array<Array<int>> data;
	int length;
	double resolution;
	void import_wave(const String &file_name);
	void display(const int &frame);
};



// 書き込みを行う
void export_wave(const String &file_name, const int &fps);
