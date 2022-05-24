#include <Siv3D.hpp> // OpenSiv3D v0.6.3

#include "form.hpp"

#define SINGLE

void Main(){
	/*
	Console << U"どの操作を行いますか(0: 波形の解析・書き込み、1: textから波形の表示)";
	int t;
	Console >> t;
	String txt_file_name, audio_file_name;
	Console << U"txtファイル名を入力してください";
	Console >> txt_file_name;
	Console << U"audioファイル名を入力してください";
	Console >> audio_file_name;
	if(t == 0){
		export_wave(txt_file_name, 30, audio_file_name);
	}
	else if(t == 1){
		WaveTextReader reader;
		reader.import_wave(txt_file_name);
		int cnt = 0;
		while(System::Update()){
			if(cnt / 2 < reader.length) reader.display(cnt / 2);
			cnt++;
		}
	}
	*/
	const String txt_file_name = U"all";
	TextWriter writer(U"../music/" + txt_file_name + U".txt");
	writer.write(U"\n");
	writer.close();
	for(int i = 0; i < 20; i++){
		const String audio_file_name = U"J{:0>2}"_fmt(i + 1);
		export_wave(txt_file_name, 30, audio_file_name);
	}
	export_wave(txt_file_name, 30, U"J01--20");
}
