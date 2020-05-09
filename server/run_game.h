#pragma once
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
#include "parse_input.h"
#include "player.h"
#include "game.h"
using namespace std;
using namespace zmq;

struct board {
	vector<vector<char>>  data{10, vector<char>(10, '~')};
};

string to_string(const board &b) {
	string retval;
	for (int i = 0; i < b.data.size(); i++) {
		for (int j = 0; j < b.data.at(i).size(); j++) {
			retval.push_back(b.data.at(i).at(j));
		}

		retval.push_back('\n');
	}

	return retval;
}

void run_game(unsigned char id = 0) {
	assert(id > 0 && id < 10);//make sure we don't use too many ports
	cout << "GAME " << id << ": starting server" << endl;
	context_t context(1);
	socket_t socket(context, ZMQ_REP);
	socket.setsockopt(ZMQ_IPV6, 1);
	int port = 1533 + id + 1;
	socket.bind("tcp://*:" + to_string(port));

	vector<Player> players;
	Game game(0);
	
	//wait for players to connect
	while (true) {
		string read = s_recv(socket);//wait until something gets sent
		vector<string> data;
		string uname;
		parse_input(read, data, uname);
	
		if (data.at(0) == "JOINING") {
			if (data.size() != 2) {
				s_send(socket, uname + "\vERR");
			}
			if (players.size() < 4) {
				players.push_back(Player(uname, size_t(stoi(data.at(1)))));
				cout << "GAME " << id << ": " << uname << "joined the game" << endl;
				s_send(socket, uname + "\vADDED");
			} else {
				s_send(socket, uname + "\vFULL");
			}
		}
	}


}
