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
#include <deque>
#include "player.h"
using namespace std;
using namespace zmq;

context_t context(1);
socket_t socket(context, ZMQ_REP);

//takes a string sent over zmq and turns it into a username and usable data
void parse_input(const string &read, vector<string> &data, string &user) {
	stringstream ss(read);
	getline(ss, user, '\v');
	while (ss) {
		string temp;
		getline(ss, temp, '\v');
		if (!ss) break;
		data.push_back(temp);
	}
	//prevent out of bounds errors if the user sends no data
	if (data.size() == 0) data.push_back("ERR");
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

int main() {
	//set up socket
	cout << "starting server...\n";
	socket.setsockopt(ZMQ_IPV6, 1);
	int port = 1533;

	socket.bind("tcp://*:" + to_string(port));//listen on port

	pair<Player, Player> players;

	string read;
	vector<string> data;
	string uname;
	socket_t p1_sock(context, ZMQ_REP);
	int p1_port = 1534;
	
	while (true) {
		try{
			p1.bind("tcp://*:" + to_string(p1_port))
		} catch (zmq::error_t) {
			p1_port++;
			continue;
		}

		break;
	}

	int p2_port = p1_port + 1;
	socket_t p2_sock(context, ZMQ_REP);
	while (true) {
		try{
			p2.bind("tcp://*:" + to_string(p2_port))
		} catch (zmq::error_t) {
			p2_port++;
			continue;
		}

		break;
	}

	while (true) {
		//wait for players
		while (open(players)) {
			read = recv(socket);
			parse_input(read, data, uname);
			if (data.at(0) == "LOGIN") {
				if (add_players(players, uname)) {
					
				}
			}
		}
	}

}
