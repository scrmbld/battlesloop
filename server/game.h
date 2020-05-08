#pragma once
#include <iostream>
#include <utility>
#include "player.h"
using namespace std;

//holds the required information for a single 1v1 game
class Game{
	private:
		pair<Player*, Player*> players;
		const size_t id;
	public:
		Game() = delete;
		Game(size_t);
		void add_player(pair<Player, Game*> &p);
		Player* p1() const;
		Player* p2() const;
		size_t get_id() const;
		bool operator<(const Game &rhs) const;
};
		
