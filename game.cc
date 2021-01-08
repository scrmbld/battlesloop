#include "board.h"
#include <iostream>
#include <utility>
#include <vector>
#include <curses.h>

int main() {
	//start ncurses
	initscr();
	noecho();

	//create boards
	std::string test_data = "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
	const int p1_y = 10, p1_x = 5, p2_y = 10, p2_x = 40;
	Board p1(p1_y, p1_x, "You", test_data);
	Board p2(p2_y, p2_x, "Enemy");
	
	p1.drawBoard();
	p2.drawBoard();
	
	refresh();
	getch();
	std::string data = p1.encodeToString();
	mvprintw(1, 1, "%c", data.at(0));
	refresh();
	getch();
	p2.updateFromString(data);
	
	p1.drawBoard();
	p2.drawBoard();
	refresh();
	getch();

	//place ships
	for (int i = 0; i < 5; i++) {
		//take input
		std::pair<int, int> selected = p1.selectTile();
		p1.changeTile(selected, 1);

		//update screen
		p1.drawBoard();
		p2.drawBoard();
		refresh();

	}

	//place ships for p2 (temporary, will not be used for online)
	for (int i = 0; i < 5; i++) {
		//take input
		std::pair<int, int> selected = p2.selectTile();
		p2.changeTile(selected, 1);

		//update screen
		p1.drawBoard();
		p2.drawBoard();
		refresh();

	}
	

	//main game loop
	while (p1.isAlive() && p2.isAlive()) {
		//p1's turn
		std::pair<int, int> selected = p2.selectTile();
		p2.attackTile(selected);
		
		//update screen
		p1.drawBoard();
		p2.drawBoard();
		refresh();
	}

	//end ncurses
	refresh();
	getch();
	clear;
	endwin();
	system("clear");
	
	std::cout << p1.isAlive() << ", " << p2.isAlive() << std::endl;
	return 0;
}
