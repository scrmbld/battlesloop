//NCURSES + ZMQ chat client
//by Kerney
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <iostream>
#include <utility>
#include <vector>
using namespace std;
using namespace zmq;

//My cool color header
#include "/public/colors.h"

//NCURSES headers
#include <locale.h>
#include <curses.h>
#include <unistd.h>

struct pos{
	bool ours;//is it our board or theirs?
	int x = 0;//x position
	int y = 0;//y position
};

void response_to_vec(const string &r, vector<string> &data) {
	stringstream sts(r);
	while (sts) {
		string temp;
		getline(sts, temp);
		if (!sts) break;
		data.push_back(temp);
	}
}

string username;
socket_t *server;
void bail_out(int s = 0) {
	s = s; //Disable warning
	clear();
	endwin(); //Turn off NCURSES

	cout << username << " has left the chat." << endl;
	if (server and username != "") {
		s_send(*server,username + ":LOGOUT");
		usleep(500'000); //Let the message time to go out
	}
	exit(EXIT_SUCCESS);
}

//To make a client using ZMQ
//Step 1. Make a socket
//Step 2. Connect to server (hostname+port)
//In a loop:
	//Step 3. Send to server
	//Step 4. Receive from server

//I decided to use colons as field separators, so no colons can be used in strings
bool hascolon(const string &s) {
	for (auto &c : s) if (c == ':') return true;
	return false;
}

void draw_board(int y, int x, const vector<vector<char>> &vec, string msg = "") {
	mvprintw(y - 1, x + 1, msg.c_str());
	for (int i = 0; i < vec.size(); i++) {
		for (int j = 0; j < vec.at(i).size(); j++) {
			mvaddch(y + j, x + i, vec.at(i).at(j));
		}
	}

	refresh();
}

pair<int, int> choose_location(int start_y, int start_x) {
	int key = 0;
	int x = start_x;
	int y = start_y;
	move(y, x);
	while (key != KEY_ENTER) {
		key = getch();
		if (key == ERR) continue;
		
		if (key == KEY_RIGHT) {
			x++;
		} else if (key == KEY_LEFT) {
			x--;
		} else if (key == KEY_DOWN) {
			y++;
		} else if (key == KEY_UP) {
			y--;
		} else if (key == 10) {//enter
			break;
		}

		//clamping
		if (x < start_x) x = start_x;
		else if (x > start_x + 9) x = start_x + 9;
		if (y < start_y) y = start_y;
		else if (y > start_y + 9) y = start_y + 9;
		move(y, x);
	}

	return {x - start_x, y - start_y};
}

string to_string(pair<int, int> p) {
	return to_string(p.first) + to_string(p.second);
}

//returns true for our turn, false for theirs, only works when first joining
bool whose_turn(string read) {
	vector<string> data;
	response_to_vec(read, data);

	for (string s : data) cout << s << endl;

	if (!data.size()) bail_out();
	if (data.size() == 1) return true;
	else {
		stringstream sts(data.at(data.size() - 1)); //clear the stringstream
		sts << data.at(data.size() - 1);
		string temp;
		getline(sts, temp, ':');
		getline(sts, temp);
		if (temp.at(0) != 'l') return false;
	}

	return true;
}

//Run the program like this: "client hostname portname"
//Example "client 55.22.11.78 2001"
//Or "client 2001" to use localhost as the host
int main (int argc, char **argv)
{
	//Step 1. Make a socket
	context_t context (1);
	socket_t socket (context, ZMQ_REQ);
	socket.setsockopt(ZMQ_IPV6,1);
	server = &socket; //Get pointer for signal handler
	signal(SIGINT,bail_out); //Disconnect nicely
	//atexit(bail_out); 

	//Step 2. Connect to server
	string hostname = "localhost";
	if (argc > 2) hostname = argv[1];
	const int DEFAULT_PORT = 2543; //Change this to any port above 1024
	int port = DEFAULT_PORT;
	//You can run it like this: client 2000 to bind it to a different port
	try {
		if (argc == 2) port = stoi(argv[1]);
		else if (argc > 2) port = stoi(argv[2]);
	} catch (...) { port = DEFAULT_PORT; }

	cout << "Connecting to server..." << endl;
	socket.connect ("tcp://"s + hostname + ":" + to_string(port));
	cout << "Please enter your name:\n";
	getline(cin,username);
	if (hascolon(username)) {
		cout << "No username can have colons.\n";
		exit(EXIT_FAILURE);
	}

	//Step 3. Send data to the server - login
	string str = username + ":LOGIN";
	s_send(socket,str);

	//Set up NCURSES
	if (setlocale(LC_ALL, "") == NULL) {
		cout << "Couldn't set locale.\n";
		exit(EXIT_FAILURE);
	}

	//make the boards
	const char EMPTY = '~', HIT = 'X', MISS = 'O', SHIP = 'S';
	vector<vector<char>> our_fleet(10, vector<char>(10, '~'));
	vector<vector<char>> enemy_fleet(10, vector<char>(10, '~'));

	initscr();
	noecho();
	cbreak(); //Disable input buffering since I will do this myself
	//curs_set(0); //Hide the cursor
	keypad(stdscr,TRUE); //Enable extended input from NCURSES
	timeout(0); //Nonblocking IO
	const int MAXFPS = 60; //Cap framerate at 30fps
	mousemask(ALL_MOUSE_EVENTS,NULL); //Enable mouse support

	int ROWS = 40, COLS = 20;
	getmaxyx(stdscr, ROWS, COLS); //Read the terminals ROWS and COLS. TODO: Check for KEY_REFRESH
	idlok(stdscr,true);
	scrollok(stdscr,true);
	setscrreg(1,ROWS-3);

	clear(); //Erase screen

	move(0,0); //Move to top left
	printw("BATTLESLOOP");

	move(ROWS-1,0); //Move to bottom, two rows up

	//Step 4. Receive data from the server
	move(1,0); //Move cursor to the second line
	string response = s_recv(socket);
	printw("%s",response.c_str());
	refresh();

	//top left line/col data for both player's boards
	const int OUR_X = 10, THEIR_X = 25, ALL_Y = 5;
	bool our_turn = whose_turn(response);//TODO: figure out whose turn it is
	while(1)
	{
		clear();
		mvprintw(0, 0, "BATTLESLOOP");
		if (our_turn) {
			mvprintw(ALL_Y + 14, OUR_X, "Your Turn");
			//print the boards
			draw_board(ALL_Y, OUR_X, our_fleet, "Your Fleet");
			draw_board(ALL_Y, THEIR_X, enemy_fleet, "Enemy Fleet");
			refresh();

			//pick a spot on our board
			pair<int, int> coords = choose_location(ALL_Y, OUR_X);

			s_send(socket, username + ":" + to_string(coords));
			string read = s_recv(socket);
			mvprintw(ALL_Y + 15, OUR_X, read.c_str());
		} else {
			mvprintw(ALL_Y + 14, OUR_X, "Opponent's Turn");
			draw_board(ALL_Y, OUR_X, our_fleet, "Your Fleet");
			draw_board(ALL_Y, THEIR_X, enemy_fleet, "Enemy Fleet");
			//TODO: opponent's turn
		}

		our_turn = !our_turn;
	}

	endwin(); //Turn off NCURSES
	return 0;
}
