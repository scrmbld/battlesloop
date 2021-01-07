#pragma once
#include <iostream>
#include <utility>
#include <vector>
#include <curses.h>

class Board {
	private:
		const int SIZE = 10;
		//maps each possible value for a position on the board to its visual representation in the game
		const std::vector<char> visual = {'~', 'S', 'M', 'H'};
		std::vector<std::vector<int>> vec; 
		//board's position on the screen
		const int start_x, start_y;
		//board's name (printed at start_x, start_y - 1)
		const std::string name;
	public:
		Board(int y, int x, std::string s);
		//prints out the board with top left corner at start_x, start_y
		//uses visual to map items in vec to what the player sees
		void drawBoard();

		//player uses arrow keys to select a spot on the board
		std::pair<int, int> selectTile();

		//get a point on the board
		int getTile(int y, int x);
		int getTile(std::pair<int, int>);
		//modify a point on the board
		void changeTile(int y, int x, int new_val);
		void changeTile(std::pair<int, int>, int new_val);
		//attack a point on the board
		void attackTile(int y, int x);
		void attackTile(std::pair<int, int>);

		//check to see if any of the board's fleet is alive
		bool isAlive(); 
};
