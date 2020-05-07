#include <iostream>
#include <signal.h>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include "draw_board.h"
using namespace std;
using namespace zmq;

socket_t *server = nullptr;
string uname;

void die(int s = 0) {
	//TODO: update this when we add back ncurses
	s = s;

	cout << "logging out" << endl;
	if (server && uname != "") {
		s_send(*server, uname + "\vLOGOUT");
		cout << "e" << endl;
		usleep(250'000);
	}
	cout << "ee" << endl;
	exit(EXIT_SUCCESS);
}

int main() {
	system("figlet BATTLESLOOP");
	cout << "\n\n\n" << "What is your username?" << endl;
	cin >> uname;
	cout << "connecting to server..." << flush;
	signal(SIGINT, die); //disconnect nicely
	/*
	   vector<vector<char>> board;
	   for (int i = 0; i < 10; i++) {
	   vector<char> vec;
	   for (int j = 0; j < 10; j++) {
	   vec.push_back('~');
	   }

	   board.push_back(vec);
	   }
	   */
	//start ncurses

	/*
	   WINDOW* win = nullptr;
	   init_board_window(1, 1, win);
	   wrefresh(win);
	   getch();
	   draw_board(board, win);
	   wrefresh(win);
	   */

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

	string response = s_recv(socket);
	cout << "found server" << endl;
	server = &socket;
	/*
	   system("clear");
	   initscr();
	   noecho();
	   clear();
	   move(0,0);
	   printw("Battlesloop\n");
	   refresh();
	   */
	const int MAXFPS = 60;//cap the frame rate
	if (response != "ADDED") {
		cout << "Something went wrong: unable to log in" << endl;
	}
	while (true) {
		s_send(socket, uname + "\v" + "\a");
		response.clear();
		response = s_recv(socket);
		usleep(1'000'000 / MAXFPS);
		if (response != "\a") {
			cout << "something went wrong" << endl;
			break;
		}
	}
	/*
	   getch();
	   clear();
	   endwin();
	   */	
	cout << response << endl;
	return 0;

}
