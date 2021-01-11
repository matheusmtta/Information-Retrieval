#include <bits/stdc++.h>

using namespace std;

int main(){
	int w = 0, id; string str;
	cout << "[";
	bool flag = false;
	while (cin >> id >> str){
		if (flag) cout << ", ";
		for (int i = 0; i < str.size(); i++)
			if (str[i] == '/' || str[i] == '.')
				w++;
		cout << w-2;
		flag = true;
	}
	cout << "]" << endl;
}