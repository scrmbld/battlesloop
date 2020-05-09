#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "board.h"
using std::string;

class Player {
	private:
		size_t uid;
		string uname;
	public:
		Player() = delete;
		Player(string name, size_t id) : uid(id), uname(name) {}
		size_t get_uid() const { return uid; }
		string get_uname() const { return uname; }
		bool operator==(const Player &rhs) const { return uname == rhs.uname && uid == rhs.get_uid(); }
};

//TODO: fix this
/*
std::ostream& operator<<(std::ostream &outs, const Player &p) {
	outs << p.get_uname();
	return outs;
}
*/
