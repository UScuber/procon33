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
vector<vector<vector<int>>> waves(20);
vector<vector<int>> allwave;
int main(){
	rep(i, 20) read(waves[i]);
	read(allwave);
	int mx_len = 0;
	rep(i, 20) mx_len = max(mx_len, (int)waves[i].size());
	mx_len = max(mx_len, (int)allwave.size());
	rep(i, 20) waves[i].resize(mx_len, vector<int>(hz));
	allwave.resize(mx_len, vector<int>(hz));
	int cnt = 0;
	constexpr double prop = 0.1;
	rep(i, mx_len){
		rep(j, hz){
			int l = 0;
			rep(k, 20) l += waves[k][i][j];
			const int r = allwave[i][j];
			const int d = abs(r - l);
			if(d > 10 && l * prop > r){
				cnt++;
				//cout << l << " " << r << "\n";
			}
		}
	}
	cerr << (double)cnt / (mx_len * hz) << "\n";
}