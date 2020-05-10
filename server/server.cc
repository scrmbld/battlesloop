#include <iostream>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <algorithm>
#include "parse_input.h"
using namespace std;
using namespace zmq;

struct Player{
	string uname;
	bool operator==(const Player &rhs) {
		return uname == rhs.uname;
	}
};

bool open(const pair<Player, Player> &p) {
	return (!p.first.uname.size() || !p.second.uname.size());
}


void remove_player(pair<Player, Player> &p, const string &r) {
	if (p.first.uname == r) p.first = Player{};
	else if (p.second.uname == r) p.second = Player{};
}

bool add_player(pair<Player, Player> &p, const string &new_player) {
	if (p.first == Player{new_player} || p.second == Player{new_player}) {
		return false;
	}
	if (!p.first.uname.size()) {
		p.first = {new_player}; 
		return true;
	}
	if (!p.second.uname.size()) {
		p.second = {new_player};
		return true;
	}

	return false;
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
	pair<Player, Player> players;
	string read;
	vector<string> data;
	string uname;

	while (true) {
		//wait for players
		cout << "SERVER: waiting for players" << endl;
		while (open(players)) {
			read = s_recv(socket);
			parse_input(read, data, uname);
			if (data.at(0) == "JOIN") {
				if (add_player(players, uname)) {
					cout << "SERVER: " << uname << " has joined the game" << endl;
					s_send(socket, uname + "\vADDED");
				} else {
					s_send(socket, uname + "\vFULL");
				}
			} else if (data.at(0) == "LOGOUT") {
				cout << "SERVER: " << uname << " has left the game" << endl;
				remove_player(players, uname);
				s_send(socket, "\a");
			} else {
				s_send(socket, "ALL\v\a");
			}
		}

		//run the game
		cout << "SERVER: players gathered, start the game" << endl;
		while (!open(players)) {
			read = s_recv(socket);
			parse_input(read, data, uname);
			if (data.at(0) == "LOGOUT") {
				cout << "SERVER: " << uname << " has left the game" << endl;
				remove_player(players, uname);
				s_send(socket, "\a");
				break;
			}

			s_send(socket, "ALL\v\a");
		}

	}
}
