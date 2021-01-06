#include "board.h"
#include <iostream>
#include <utility>
#include <vector>
#include <curses.h>

Board::Board(int y, int x, std::string s) : start_y(y), start_x(x), name(s) {
	vec.resize(10);
	for (auto &v : vec) {
		v.resize(10);
		for (int &i : v) {
			i = 0;
		}
	}
}
//prints out the board with top left corner at start_x, start_y
//uses visual to map items in vec to what the player sees
void Board::drawBoard() {
	mvprintw(start_y - 1, start_x, name.c_str());
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			mvaddch(i + start_y, j * 2 + start_x, visual.at(vec.at(i).at(j)));
		} 
	}

	refresh();
}

std::pair<int, int> Board::selectTile() {
	int key = 0;
	int y = 0, x = 0;
	move(start_y + y, start_x + x * 2);
	while (key != 10) {
		key = getch();
		if (key == ERR) continue;

		//arrow keys
		if (key == 67) {
			x++;
		} else if (key == 68) {
			x--;
		} else if (key == 66) {
			y++;
		} else if (key == 65) {
			y--;
		} else if (key == 10) break; 

		//clamping
		if (x < 0) x = 0;
		if (y < 0) y = 0;
		if (x > 9) x = 9;
		if (y > 9) y = 9;
		move(start_y + y, start_x + x * 2);
	}

	return {y, x};
}

