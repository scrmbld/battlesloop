#pragma once
#include <iostream>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <algorithm>
#include <stdexcept>
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

void run_game(size_t id = 0) {
	assert(id >= 0 && id < 10);//make sure we don't use too many ports
	cout << "GAME " << id << ": starting server" << endl;
	context_t context(1);
	socket_t socket(context, ZMQ_REP);
	socket.setsockopt(ZMQ_IPV6, 1);
	int port = 1533 + id + 1;
	socket.bind("tcp://*:" + to_string(port));

	string read = s_recv(socket);
	string uname;
	vector<string> data;
	parse_input(read, data, uname);
	if (uname == "server" && data.at(0) == "?") {
		cout << "GAME: connected to matchmaking" << endl;
		s_send(socket, to_string(id) + "\vAVAILABLE");
	} else {
		s_send(socket, to_string(id) + "\vERR");
	}

	vector<Player> players;
	Game game(0);

	//wait for players to connect
	while (true) {
		try {
			string read = s_recv(socket);//wait until something gets sent
		} catch (const zmq::error_t) {
			cout << "it's the game" << endl;
		}
		cout << "r" << endl;
		vector<string> data;
		string uname;
		parse_input(read, data, uname);
		cout << uname << endl;
		if (uname == "server") {
			if (data.at(0) == "?") {
				cout << "GAME: server querying for openings" << endl;
				if (players.size() < 4) {
					s_send(socket, uname + "\vAVAILABLE");
				} else {
					s_send(socket, to_string(id) + "\v\a");
				}
			} else {
				s_send(socket, to_string(id) + "\v\a");
			}
			continue;
		} else if (data.at(0) == "JOINING") {
			if (players.size() < 4) {
				players.push_back(Player(uname, players.size()));
				cout << "GAME " << id << ": " << uname << "joined the game" << endl;
				s_send(socket, uname + "\vADDED");
			} else {
				s_send(socket, uname + "\vFULL");
			}

			continue;
		}
		
		s_send(socket, to_string(id) + "\v\a");
	}


}
