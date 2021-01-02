#include <iostream>
#include <curses.h>

int main() {
	//start ncurses
	initscr();
	noecho();
	printw("hello world!");
	
	//end ncurses
	refresh();
	getch();
	clear;
	endwin();
	system("clear");
	return 0;
}
