#include <iostream>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <list>
#include <sstream>
#include <algorithm>
#include "player.h"
#include "game.h"
#include "board.h"
using namespace std;
using namespace zmq;

bool has_one(Game &g) {
	return g.p1() && !g.p2();
}

int main() {
	//set up socket
	cout << "starting server...\n";
	context_t context(1);
	socket_t socket(context, ZMQ_REP);
	socket.setsockopt(ZMQ_IPV6, 1);
	int port = 1533;
	
	socket.bind("tcp://*:" + to_string(port));//listen on port

	//prepare for players
	unordered_map<string, pair<Player, Game*>> players;
	list<Game> games;
	size_t next_gid = 1;
	//wait for players to connect
	while (true) {
		string read = s_recv(socket);//wait until something gets sent
		stringstream ss(read);
		string uname;
		getline(ss, uname, '\v');//use vertical tabs to separate data
		string data;
		getline(ss, data, '\v');

		//if the player is just joining:
		if (data == "LOGIN") {
			cout << "SERVER: " << read << endl;
			if (players.count(uname) == 0) {//don't allow duplicates
				pair<Player, Game*> new_player = {Player(uname, players.size()), nullptr};
				players.emplace(uname, new_player);
				s_send(socket, uname + "\vADDED");
				continue;
			} else {
				cout << "SERVER: user already exists" << endl;
				s_send(socket, uname + "\vERR");
				continue;
			}
		} else if (data == "LOGOUT") {
			players.erase(uname);
		} else if (data == "\a") { //\a indicates empty message
		} else if (data == "SEARCH") {//the client wishes to join a game
			auto it = find_if(games.begin(), games.end(), has_one);
			if (it != games.end()) {
				it->add_player(players.at(uname));
				s_send(socket, uname + "\vJOINED");
				continue;
			} else {
				games.push_back(Game(next_gid));
				games.back().add_player(players.at(uname));
				next_gid++;
			}
		} else {
			//TODO: add game logic
		}
		//TODO: update player on game state
		//for now just send them whatever
		s_send(socket, "\a");
	}
}
