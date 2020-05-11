#pragma once
#include <iostream>
#include <vector>
#include <unistd.h>
#include <ncurses.h>
using namespace std;

const int UP = 65;
const int DOWN = 66;
const int LEFT = 68;
const int RIGHT = 67;
const int ESCAPE = 27;
const int ENTER = 10;
const char BATTLESHIP_KEY = 'b';
const char SUBMARINE_KEY = 's';
const char DESTROYER_KEY = 'd';
const char AIRCRAFT_CARRIER_KEY = 'a';

class Board {
	private:
		vector<vector<char>> board;
	public:
		static const char WATER = '~';
		static const char CURSOR = '*';
		static const char DESTROYER = 'D';
		static const char SUBMARINE = 'S';
		static const char BATTLESHIP = 'B';
		static const char AIRCRAFT_CARRIER = 'A';
		static const size_t SIZE = 10;
		void fill_board() {
			board.resize(SIZE);
			for (auto &v : board) v.resize(SIZE,'~');
			for (size_t i = 0; i < SIZE; i++) {
				for (size_t j= 0; j < SIZE; j++) {
					if (i == SIZE/2 && j == SIZE/2) board.at(i).at(j) = CURSOR;
				}
			}
		}
		//Creates a box with letters for some reason
		void init_board_window(int y, int x, WINDOW* &board) {
			board = newwin(12, 22, y, x);
			box(board, 0, 0);
		}
		
		void draw_board(WINDOW *win) {
			for (int i = 0; i < board.size(); i++) {
				for (int j = 0; j < board.size(); j++) {
					mvwaddch(win, i + 1, j * 2 + 1, board.at(i).at(j)); //Ian's starter code
				}
			}
		}

		
		void move_cursor(int &x, int &y, int ch) {
			if (ch == UP) {
				y--;
				if (y < 0) y = 0;
				if (get(x,y) == WATER) {
					set(x,y,CURSOR);
					if (get(x, y+1) == CURSOR) set(x, y+1, WATER);
				}
			}
			if (ch == DOWN) {
				y++;
				if (y > SIZE) y = SIZE - 1;
				if (get(x,y) == WATER) {
					set(x,y,CURSOR);
					if (get(x, y-1) == CURSOR) set(x, y-1, WATER);
				}
			}
			if (ch == LEFT) {
				x--;
				if (x < 0) x = 0;
				if (get(x,y) == WATER) {
					set(x,y,CURSOR);
					if (get(x+1, y) == CURSOR) set(x+1, y, WATER);
				}
			}
			if (ch == RIGHT) {
				x++;
				if (x > SIZE) x = SIZE - 1;
				if (get(x,y) == WATER) {
					set(x,y,CURSOR);
					if (get(x-1, y) == CURSOR) set(x-1, y, WATER);
				}
			}
		}
		
		void place_ship(int &x, int &y, char c) {
			if (c == BATTLESHIP_KEY) set(x,y, BATTLESHIP);
			if (c == DESTROYER_KEY) set(x,y, DESTROYER);
			if (c == SUBMARINE_KEY) set(x,y, SUBMARINE);
			if (c == AIRCRAFT_CARRIER_KEY) set(x,y, AIRCRAFT_CARRIER);
		}

		void set(int x, int y, char c) {
			board.at(y).at(x) = c;
		}

		char get(int x, int y) {
			return board.at(y).at(x);
		}

		Board () {
			fill_board();
		}
};
