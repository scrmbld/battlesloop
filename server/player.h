#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "board.h"
using std::string;

class Player {
	private:
		const size_t uid;
	public:
		const string uname;
		Player() = delete;
		Player(string name, size_t id) : uid(id), uname(name) {}
};

/*
std::ostream& operator<<(std::ostream &outs, const Player &p) {
	outs << p.uname;
	return outs;
}
*/
