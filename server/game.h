#pragma once
#include <iostream>
#include <utility>
#include "player.h"
using namespace std;

//holds the required information for a single 1v1 game
class Game{
	private:
		pair<string, string> players;
		const size_t id;
	public:
		Game() = delete;
		Game(size_t);
		void add_player(pair<Player, Game*> &p);
		void add_player(const Player &p);
		void remove_player(bool);
		void remove_player(const Player &p);
		string p1() const;
		string p2() const;
		bool in_game(const Player &p) const;
		bool in_game(const string &p) const;
		size_t get_id() const;
		bool operator<(const Game &rhs) const;
};
		
