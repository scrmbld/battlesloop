#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "board.h"
using std::string;

class Player {
	private:
		const size_t uid;
		Player *opponent;
		//TODO: add a pointer to the player's board
		bool playing = false;
	public:
		const string uname;
		Player() = delete;
		Player(string name, size_t id) : uid(id), uname(name) {}
		bool in_game() const { return playing; }
		void joined() { playing = true; }
};

/*
std::ostream& operator<<(std::ostream &outs, const Player &p) {
	outs << p.uname;
	return outs;
}
*/
