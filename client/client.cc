//NCURSES + ZMQ chat client
//by Kerney
#include <zmq.hpp>
#include <zhelpers.hpp>
#include <string>
#include <iostream>
using namespace std;
using namespace zmq;

//My cool color header
#include "/public/colors.h"

//NCURSES headers
#include <locale.h>
#include <curses.h>
#include <unistd.h>

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
	const int DEFAULT_PORT = 1533; //Change this to any port above 1024
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
	printw("CSCI4X Chat");

	move(ROWS-1,0); //Move to bottom, two rows up
	printw("%s: TYPE YOUR CHAT HERE",username.c_str());

	//Step 4. Receive data from the server
	move(1,0); //Move cursor to the second line
	string response = s_recv(socket);
	printw("%s",response.c_str());
	refresh();

	int x = 0, y = 0; //Location in the world
	string current_str; //What the user is currently typing
	const int OUR_X = 10, THEIR_X = 25, ALL_Y = 5;
	while(1)
	{
		//print the boards
		draw_board(ALL_Y, OUR_X, our_fleet, "Your Fleet");
		draw_board(ALL_Y, THEIR_X, enemy_fleet, "Enemy Fleet");
		
		int ch = getch();
		int cursor_y = 0, cursor_x = 0;
		getyx(stdscr,cursor_y,cursor_x); //Gets current cursor location
		/*
		move(ROWS-1,0);
		printw("Cursor location: %i %i",cursor_y,cursor_x);
		refresh();
		move(cursor_y,cursor_x);
        */
		if (ch == ERR) { //No keystroke
            ; //Do nothing
        }
		//TODO: Add mouse support, I'm commenting this out for now
		else if(ch == KEY_MOUSE) { //Handle mouse events
			MEVENT mouse; //Struct for holding mouse events
			getmouse(&mouse);

			//move(0,0);
			//clrtoeol();
			//printw("%d\t%d\n",mouse.y,mouse.x);
			//mvprintw(mouse.y,mouse.x,"🤣"); //Move + printw
			//refresh();
		}
		//Handle arrow keys
		//TODO: Have them scroll the chat buffer, they don't do anything right now
        else if (ch == KEY_RIGHT) {
            x++;
			if (x >= COLS) x = COLS-1;
        }
        else if (ch == KEY_LEFT) {
            x--;
            if (x < 0) x = 0;
        }
        else if (ch == KEY_UP) {
            y--;
            if (y < 0) y = 0;
        }
        else if (ch == KEY_DOWN) {
            y++;
			if (y >= ROWS) y = ROWS - 1;
        }
		//Handle special keys like delete and enter
		else if (ch == KEY_DC or ch == KEY_BACKSPACE or ch == '') { //Delete keys
			if (current_str.size())
				current_str.pop_back();
			move(ROWS-1,0); //Move to bottom
			clrtoeol(); //Erase the whole line
			printw("%s: %s\n",username.c_str(),current_str.c_str());
			move(cursor_y,cursor_x); //Put cursor back where it was
			refresh();
		}
		//ctrl-u erases everything typed
		else if (ch == KEY_DL) { //Delete line
			current_str.clear(); //Erase everything we've typed
		}
		else {
			char c = (char) ch; //Convert ch to char
			if (isprint(c) and ch != KEY_ENTER and c != ':') { //Make sure it's a printable character before adding it
				current_str += c; //Add it to the string we're typing
				move(ROWS-1,0); //Move to bottom
				printw("%s: %s\n",username.c_str(),current_str.c_str());
				move(cursor_y,cursor_x); //Put cursor back where it was
			}
		}

		//Send a line of chat if they hit enter
		if (ch == KEY_ENTER or ch == '\n') {
			if (current_str == "QUIT" or current_str == "quit") exit(0);
			s_send(socket,username + ":" + current_str);
			current_str.clear(); //Erase the current line
			move(ROWS-1,0); //Move to bottom
			printw("%s: %s\n",username.c_str(),current_str.c_str());
			move(cursor_y,cursor_x); //Put cursor back where it was
		}
		else //Heartbeat the server to get updates
			s_send(socket,username + ":");
		string response = s_recv(socket); //Get update from server
		if (response != "") //Ignore empty heartbeat
			printw("%s",response.c_str());

		if (ch == ERR and response == "") 
			; //Nothing happened, don't refresh
		else
			refresh();
		
		usleep(1'000'000 / MAXFPS); //Cap frame rate at FPS
	}
	
	endwin(); //Turn off NCURSES
	return 0;
}
