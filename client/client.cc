#include <iostream>
#include <signal.h>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <sstream>
#include "draw_board.h"
#include "player.h"
using namespace std;
using namespace zmq;

socket_t *server = nullptr;
string uname;

void start_ncurses() {
	system("clear");
	initscr();
	noecho();
	clear();
}

void stop_ncurses(){
	clear();
	endwin();
	system("clear");
}

void parse_input(const string &input, vector<string> &data) {
	data.clear();
	stringstream ss(input);
	string tgt;
	getline(ss, tgt, '\v');
	if (tgt != uname && tgt != "ALL") {
		data.push_back("ERR");
		return;
	}
	while (ss) {
		string temp;
		getline(ss, temp, '\v');
		if (!ss) break;
		data.push_back(temp);
	}
}

void die(int s = 0) {
	//TODO: update this when we add back ncurses
	s = s;
	stop_ncurses();

	cout << "logging out" << endl;
	if (server && uname != "") {
		try{
			s_send(*server, uname + "\vLOGOUT");
		} catch (const zmq::error_t e) {
			s_recv(*server);
			s_send(*server, uname + "\vLOGOUT");
		}
		usleep(250'000);
	}
	exit(EXIT_SUCCESS);
}

int main() {
	system("figlet BATTLESLOOP");
	cout << "\n\n\n" << "What is your username?" << endl;
	cin >> uname;
	Player self(uname, 0);
	cout << "connecting to server..." << flush;
	signal(SIGINT, die); //disconnect nicely

	//make a socket
	context_t context(1);
	socket_t socket(context, ZMQ_REQ);
	socket.setsockopt(ZMQ_IPV6, 1);
	socket.setsockopt(ZMQ_CONNECT_TIMEOUT, 2000);
	string hostname = "localhost";
	int port = 1533;

	//connect to server
	socket.connect("tcp://" + hostname + ":" + to_string(port));
	s_send(socket, uname + "\v" + "JOIN");

	//start ncurses
	int line = 0;
	start_ncurses();
	move(0,line);
	printw("Battlesloop\n");
	line+= 3;
	refresh();
	const int MAXFPS = 60;//cap the frame rate

	//
	string read = s_recv(socket);
	vector<string> data;
	parse_input(read, data);
	if (data.at(0) == "ADDED") {//if the server lets us in
		server = &socket;
	} else {
		exit(1);
	}
	

	//main game loop
	while (true) {
		s_send(socket, uname + "\v\a");
		read = s_recv(socket);
	}
	
	die();
}
