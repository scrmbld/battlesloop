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
		//construct empty board taking arguments for y position, x position, and name
		Board(int y, int x, std::string name);
		Board(std::pair<int, int> pos, std::string name);
		//construct non-empty board from a data string
		Board(int y, int x, std::string name, std::string data);
		Board(std::pair<int, int> pos, std::string name, std::string data);
		
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

		//encode the board's data to a string
		std::string encodeToString();
		//update the board's data from a string
		void updateFromString(std::string new_data); 
};
