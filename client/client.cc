#include <iostream>
#include <signal.h>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include <sstream>
using namespace std;
using namespace zmq;

socket_t *server = nullptr;
context_t context(1);
socket_t socket(context, ZMQ_REQ);
string uname;
bool in_game = false;
int line = 0; //text positioning
const int MAXFPS = 60;
#include "draw_board.h"
#include "player.h"
#include "parse_input.h"


void stop_ncurses() {
	clear();
	endwin();
}

void die(int s = 0) {
	s = s;
	stop_ncurses();

	cout << "logging out" << endl;
	if (server && uname != "") {
		s_send(*server, uname + "\vLOGOUT");
		usleep(250'000);
	}
	exit(EXIT_SUCCESS);
}

void init_ncurses() {
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
}

string start_connection() {
	//make a socket
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
	return read;
}

void join_match(string read) {
	vector<string> data;//for data received over network
	parse_input(read, data);
	//contact matchmaking server
	if (data.at(0) == "ADDED") {
		mvprintw(line, 0, "Connected successfully");
		refresh();
		line++;
	} else {
		mvprintw(line, 0, "Something went wrong: unable to log in");
		refresh();
		exit(EXIT_FAILURE);
		line++;
	}
	refresh();

	//find a match
	mvprintw(line, 0, "looking for a match...");
	refresh();
	line++;
	while (!in_game) {
		s_send(socket, uname + "\vSEARCH");
		read = s_recv(socket);
		parse_input(read, data);
		usleep(1'000'000 / MAXFPS);
		if (data.at(0) == "JOINED") {
			in_game = true;

			mvprintw(line, 0, "found game");
			refresh();
			line++;
			break;
		}
	}
	//TODO: connect to match

}



int main() {
	system("figlet BATTLESLOOP");
	cout << "\n\n\n" << "What is your username?" << endl;
	cin >> uname;
	cout << "connecting to server..." << flush;
	signal(SIGINT, die); //disconnect nicely



	//connect to matchmaking
	string read = start_connection();
	//start ncurses
	init_ncurses();
	//find a match
	join_match(read);

	//stay connected
	while (true) {
		s_send(socket, uname + "\v\a");
		string read = s_recv(socket);
	}
}
