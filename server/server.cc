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

//set up socket
context_t context(1);
socket_t socket(context, ZMQ_REP);
int port = 1533;

struct Player{
	string uname;
	bool operator==(const Player &rhs) {
		return uname == rhs.uname;
	}

	vector<vector<char>> board{10, (vector<char>(10, '*'))};
	char at(const pair<int, int> p) { return board.at(p.first).at(p.second); }
};

pair<Player, Player> players;

string to_string(const vector<vector<char>> &vec) {
	string s;
	for (auto &v : vec) {
		for (char c : v) {
			s.push_back(c);
		}
		s.push_back('\n');
	}

	return s;
}

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

void logout(string uname, bool read = false) {
	cout << "SERVER: " << uname << " has left the game" << endl;
	remove_player(players, uname);
	try {
		s_send(socket, "\a");
	} catch(const zmq::error_t e) {
		s_recv(socket, ZMQ_NOBLOCK);
		s_send(socket, "\a");
	}

	if (read) s_recv(socket);
}

pair<int, int> turn_input(const string &p) {//gets the input from the specified player for the turn
	string read;
	vector<string> data;
	string uname;

	int row, col = 0;

	while (true) {
		try {
			s_send(socket, uname + "\vTURN");
		} catch (const zmq::error_t e) {
			s_recv(socket);
			s_send(socket, uname + "\vTURN");
		}
		read = s_recv(socket);
		parse_input(read, data, uname);
		if (uname != p) {
			if (data.at(0) == "LOGOUT") {
				logout(uname, true);
			}
			continue;
		}

		if (data.size() == 2 && data.at(0) == "FIRE") {
			row = data.at(1).at(0) - 'a';
			col = data.at(1).at(1) - '0';
			break;
		} else if (data.at(0) == "LOGOUT") {
			logout(uname, true);
			break;
		}else {
			//for (const string &s : data) cout << s;
		}
	}

	return {row, col};
}

int main() {
	cout << "starting server...\n";
	socket.setsockopt(ZMQ_IPV6, 1);
	socket.bind("tcp://*:" + to_string(port));//listen on port
	//prepare for players
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
				logout(uname);
			} else {
				s_send(socket, "ALL\v\a");
			}
		}

		//run the game
		cout << "SERVER: players gathered, start the game" << endl;
		bool turn = false; //false = players.first's turn, true == player.second's turn
		while (!open(players)) {
			turn = !turn;
			parse_input(read, data, uname);
			if (data.at(0) == "LOGOUT") {
				logout(uname);
				cout << "logged out successfully" << endl;
			}
			pair<int, int> coords = {0, 0};

			if (!turn) {
				string msg;
				//get player input
				cout << "1" << endl;
				coords = turn_input(players.first.uname);
				cout << "1" << endl;
				//check for hit/miss
				if (players.first.at(coords) == '~') {//miss
					msg = string(players.first.uname + "\vMISS\v" + to_string(coords.first) + to_string(coords.second));
					cout << "p1 misses" << endl;
				} else {//hit
					cout << "p1 hits" << endl;
					msg =  string("ALL\vHIT\v" + players.first.uname + "\v" + to_string(coords.first) + to_string(coords.second));
				}
				
				while (true) {
					s_send(socket, msg);
					read = s_recv(socket);
					if (uname == players.second.uname) break;
				}
				continue;
			} else{
				string msg;
				//DRY: do repeat yourself
				coords = turn_input(players.second.uname);
				if (players.second.at(coords) == '~') {
					cout << "p2 misses" << endl;
					msg = string(players.second.uname + "\vMISS\v" + to_string(coords.first) + to_string(coords.second));
				} else {
					cout << "p2 hits" << endl;
					msg = string("ALL\vHIT\v" + players.second.uname + "\v" + to_string(coords.first) + to_string(coords.second));
				}
				
				while (true) {
					s_send(socket, msg);
					read = s_recv(socket);
					if (uname == players.first.uname) break;
				}
				
				continue;
			} 

			cout << coords.first << ", " << coords.second << endl;

			s_send(socket, "ALL\v\a");
		}

	}
}
