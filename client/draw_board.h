#pragma once
#include <ncurses.h>
#include <unistd.h>
#include <vector>
using namespace std;

//creates a new window for a game board
void init_board_window(int y, int x, WINDOW* &board) {
	board = newwin(12, 22, y, x);
	box(board, 0, 0);
}

void draw_board(const vector<vector<char>> &board, WINDOW *win) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			mvwaddch(win, i + 1, j * 2 + 1, board.at(i).at(j));
		}
	}
}
