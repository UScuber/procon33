#include <Siv3D.hpp> // OpenSiv3D v0.6.3

#include "form.hpp"


void Main(){
	Console << U"どの操作を行いますか(0: 波形の解析・書き込み、1: textから波形の表示)";
	int t;
	Console >> t;
	String file_name;
	Console << U"ファイル名を入力してください";
	Console >> file_name;
	if(t == 0){
		export_wave(file_name, 30);
	}
	else if(t == 1){
		WaveTextReader reader;
		reader.import_wave(file_name);
		int cnt = 0;
		while(System::Update()){
			if(cnt / 2 < reader.length) reader.display(cnt / 2);
			cnt++;
		}
	}
}
