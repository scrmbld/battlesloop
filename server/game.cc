#include "game.h"
#include "player.h"
using namespace std;

//member functions for Game
Game::Game(size_t new_id) : id(new_id) {

}

void Game::add_player(pair<Player, Game*> &p) {
	p.second = this;
	if (!players.first) {
		players.first = &p.first;
		return;
	}

	players.second = &p.first;
}

Player* Game::p1() const { return players.first; }
Player* Game::p2() const { return players.second; }
size_t Game::get_id() const { return id; }

bool Game::operator<(const Game &rhs) const {
	return id < rhs.get_id();
}
