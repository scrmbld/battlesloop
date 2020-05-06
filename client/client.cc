#include <iostream>
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <vector>
#include <ncurses.h>
#include <unistd.h>
using namespace std;
using namespace zmq;

int main() {
	system("clear");
	initscr();
	noecho();
	clear();
	move(0,0);
	printw("Hello World!\n");
	refresh();
	
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
