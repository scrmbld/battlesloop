#include "game.h"
#include "player.h"
using namespace std;

//member functions for Game
Game::Game(size_t new_id) : id(new_id) {

}

void Game::add_player(const Player &p) {
	if (!players.first.size()) {
		players.first = p.get_uname();
		return;
	}

	players.second = p.get_uname();
}

void Game::remove_player(bool tgt) {
	if (tgt) players.second = "";
	else players.first = "";
}

void Game::remove_player(const Player &p) {
	if (p.get_uname() == players.first) players.first ="";
	else if (p.get_uname() == players.second) players.second = "";
}

bool Game::in_game(const Player &p) const {
	return (p.get_uname() == players.first || p.get_uname() == players.second);
}
bool Game::in_game(const string &p) const {
	return (p == players.first || p == players.second);
}

string Game::p1() const { return players.first; }
string Game::p2() const { return players.second; }
size_t Game::get_id() const { return id; }

