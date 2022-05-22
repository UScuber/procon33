#include <bits/stdc++.h>
#define rep(i, n) for(int i = 0; i < (n); i++)
using namespace std;

constexpr int hz = 600;
void read(vector<vector<int>> &res){
	int n;
	double resolution;
	cin >> n >> resolution;
	res.assign(n, vector<int>(hz));
	rep(i, n) rep(j, hz) cin >> res[i][j];
}
int main(){
	vector<vector<int>> wave1, wave2, wave12;
	read(wave1);
	read(wave2);
	read(wave12);
	const int mx_len = max({ wave1.size(), wave2.size(), wave12.size() });
	wave1.resize(mx_len, vector<int>(hz));
	wave2.resize(mx_len, vector<int>(hz));
	wave12.resize(mx_len, vector<int>(hz));
	int cnt = 0;
	constexpr double prop = 0.3;
	rep(i, mx_len){
		rep(j, hz){
			const int l = wave1[i][j] + wave2[i][j];
			const int r = wave12[i][j];
			const int d = abs(r - l);
			if(d > 4 && l * prop > r){
				cnt++;
				cout << l << " " << r << "\n";
			}
		}
	}
	cerr << (double)cnt / (mx_len * hz) << "\n";
}