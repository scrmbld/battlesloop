#include <iostream>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
#include "draw_board.h"
using namespace std;
using namespace zmq;

int main() {
	vector<vector<char>> board;
	for (int i = 0; i < 10; i++) {
		vector<char> vec;
		for (int j = 0; j < 10; j++) {
			vec.push_back('~');
		}

		board.push_back(vec);
	}
	
	system("clear");
	initscr();
	noecho();
	clear();
	move(0,0);
	printw("Hello World!\n");
	refresh();
	WINDOW* win = nullptr;
	init_board_window(1, 1, win);
	wrefresh(win);
	getch();
	draw_board(board, win);
	wrefresh(win);

	
	context_t context(1);
	socket_t socket(context, ZMQ_REQ);
	socket.setsockopt(ZMQ_IPV6, 1);
	string hostname = "localhost";
	int port = 1533;
	
	socket.connect("tcp://" + hostname + ":" + to_string(port));
	s_send(socket, "h");

	string response = s_recv(socket);
	move(40, 0);
	printw("recieved %s", response.c_str());

	getch();
	clear();
	endwin();
	
	return 0;
}
