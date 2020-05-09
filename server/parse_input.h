#include <iostream>
#include <vector>
#include <sstream>
using namespace std;

//takes data sent over zmq and turns it into something usable
void parse_input(const string &read, vector<string> &data, string &user) {
	stringstream ss(read);
	getline(ss, user, '\v');
	while (ss) {
		string temp;
		getline(ss, temp, '\v');
		if (!ss) break;
		data.push_back(temp);
	}
	//prevent out of bounds errors if the user sends no data
	if (data.size() == 0) data.push_back("ERR");
}
