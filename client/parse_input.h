#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

void parse_input(const string &input, vector<string> &data) {
	data.clear();
	stringstream ss(input);
	string tgt;
	getline(ss, tgt, '\v');
	if (tgt == "\a") {
		data.push_back("\a");
		return;
	}
	if (tgt != uname) {
		data.push_back("ERR");
		return;
	}
	while (ss) {
		string temp;
		getline(ss, temp, '\v');
		if (!ss) break;
		data.push_back(temp);
	}

	if (data.size() == 0) {
		data.at(0) == "ERR";//prevent out of bounds on improper input
	}
}
