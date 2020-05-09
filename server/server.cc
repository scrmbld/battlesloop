#include <iostream>
#include <thread>
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
#include "board.h"
#include "run_game.h"
using namespace std;
using namespace zmq;


int main() {
	//set up socket
	cout << "starting server...\n";
	context_t context(1);
	socket_t socket(context, ZMQ_REP);
	socket.setsockopt(ZMQ_IPV6, 1);
	int port = 1533;
	
	socket.bind("tcp://*:" + to_string(port));//listen on port

	//prepare for players
	unordered_map<string, Player> players;
	deque<Player> waiting;

	//wait for players to connect
	while (true) {
		string read = s_recv(socket);//wait until something gets sent
		vector<string> data;
		string uname;
		parse_input(read, data, uname);

		//if the player is just joining:
		if (data.at(0) == "LOGIN") {
			cout << "SERVER: " << uname << " joined the game" << endl;
			if (players.count(uname) == 0) {//don't allow duplicates
				players.emplace(uname, Player(uname, players.size()));//add the player to the database
				s_send(socket, uname + "\vADDED");
				continue;
			} else {
				cout << "SERVER_ERROR: user " << uname << " already exists" << endl;
				s_send(socket, uname + "\vERR");
				continue;
			}
		} else if (data.at(0) == "LOGOUT") {
			Player to_delete(uname, players.at(uname).get_uid());
			//delete them from both lists
			players.erase(uname);
			remove(waiting.begin(), waiting.end(), to_delete);
			cout << "SERVER: " << uname << " has left the game" << endl;
		} else if (data.at(0) == "\a") { //\a indicates empty message
		} else if (data.at(0) == "SEARCH") {//the client wishes to join a game
			if (find(waiting.begin(), waiting.end(), players.at(uname)) == waiting.end()) {
				waiting.push_back(players.at(uname));
				cout << "SERVER: " << uname << " was added to the waitlist" << endl;
				s_send(socket, uname + "\vWAIT");
				continue;
			} 

		} else {
			//TODO: add game logic
		}
		//TODO: update player on game state
		//TODO: handle matchmaking

		s_send(socket, "\a");
	}
}
