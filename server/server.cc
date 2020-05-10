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
#include "board.h"
#include "run_game.h"
#include <thread>
using namespace std;
using namespace zmq;

int port = 1533;

context_t rooms(1);
class room {
	public:
		socket_t sock;
		size_t id;
		bool available = false;
		room() = delete;
		room(size_t new_id) : sock(rooms, ZMQ_REQ) {
			id = new_id;
			sock.setsockopt(ZMQ_IPV6, 1);
			sock.connect("tcp://localhost:" + to_string(port + 1 + id));
		}
};

int main() {
	//set up socket
	cout << "starting server...\n";
	context_t context(1);
	socket_t socket(context, ZMQ_REP);
	socket.setsockopt(ZMQ_IPV6, 1);
	socket.bind("tcp://*:" + to_string(port));//listen on port

	//create the game server
	thread game(run_game, 0);
	room room_1(0);
	s_send(room_1.sock, "server\v?");
	s_recv(room_1.sock);

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
		//TODO: handle matchmaking


		if (waiting.size()) {
			try{
				s_send(room_1.sock, "server\v?");
			} catch (const zmq::error_t e) {
				cout << "it's the server" << endl;
				cout << e.what() << endl;
			}
			read = s_recv(room_1.sock);
			parse_input(read, data, uname);
			if (data.at(0) == "AVAILABLE") {
				room_1.available = true;
				cout << "SERVER: game open" << endl;
			}

		}

		if (room_1.available == true) {
			s_send(socket, waiting.at(0).get_uname() + "\vJOIN\v" + to_string(port + 1 + room_1.id));
			room_1.available = false;
			continue;
		}

		s_send(socket, "\a");
	}
}
