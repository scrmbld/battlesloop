#pragma once
#include <iostream>
#include <vector>
#include <string>
using std::string;

class Player {
	private:
		const size_t uid;
		Player *opponent;
		//TODO: add a pointer to the player's board
	public:
		const string uname;
		Player() = delete;
		Player(string name, size_t id) : uid(id), uname(name) {}
		
};

std::ostream& operator<<(std::ostream &outs, const Player &p) {
	outs << p.uname;
	return outs;
}
