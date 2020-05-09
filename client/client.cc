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


void parse_input(const string &input, vector<string> &data) {
	data.clear();
	stringstream ss(input);
	string tgt;
	getline(ss, tgt, '\v');
	if (tgt == "\a") {
		data.push_back("\a");
		return;
	}
	if (tgt != uname) {
		data.push_back("ERR");
		return;
	}
	while (ss) {
		string temp;
		getline(ss, temp, '\v');
		if (!ss) break;
		data.push_back(temp);
	}

	if (data.size() == 0) {
		data.at(0) == "ERR";//prevent out of bounds on improper input
	}
}

void die(int s = 0) {
	//TODO: update this when we add back ncurses
	s = s;
	clear();
	endwin();
	
	cout << "logging out" << endl;
	if (server && uname != "") {
		s_send(*server, uname + "\vLOGOUT");
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
	s_send(socket, uname + "\v" + "LOGIN");

	string read = s_recv(socket);
	vector<string> data;
	parse_input(read, data);
	cout << "found server" << endl;
	server = &socket;

	//start ncurses
	int line = 0;
	system("clear");
	initscr();
	noecho();
	clear();
	move(0,line);
	printw("Battlesloop\n");
	line+= 3;
	refresh();
	const int MAXFPS = 60;//cap the frame rate


	if (data.at(0) == "ADDED") {
		mvprintw(line, 0, "Connected successfully");
		line++;
	} else {
		mvprintw(line, 0, "Something went wrong: unable to log in");
		exit(EXIT_FAILURE);
		line++;
	}
	refresh();


	while (true) {
		if (!self.in_game()) {
			mvprintw(line, 0, "Searching for a match");
			line++;
			refresh();
		}
		while (!self.in_game()) {
			s_send(socket, uname + "\vSEARCH");
			read = s_recv(socket);
			parse_input(read, data);
			usleep(1'000'000 / MAXFPS);
			if (data.at(0) == "JOINED") {
				self.joined();
				
				mvprintw(line, 0, "Joined game ");
				refresh();
				line++;
				break;
			}
		}
		s_send(socket, uname + "\v" + "\a");
		read = s_recv(socket);
		parse_input(read, data);
		usleep(1'000'000 / MAXFPS);
	}
	refresh();
	clear();
	endwin();
	cout << data.at(0) << endl;
	return 0;

}
