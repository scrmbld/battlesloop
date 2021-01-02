#include <iostream>
#include <vector>
#include <curses.h>

class Board {
	private:
		const std::vector<char> visual = {'~', 'S', 'M', 'H'};//maps each possible value for a position on the board to its visual representation in the game
		std::vector<std::vector<int>> vec;//stores locations on the board
	public:
		Board() {
			vec.resize(10);
			for (auto &v : vec) {
				v.resize(10);
			}
		}

		//prints out the board with top left corner at start_x, start_y
		//uses visual to map items in vec to what the player sees
		void draw_board(int start_y, int start_x) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					mvaddch(i + start_y, j + start_x, visual.at(vec.at(i).at(j)));
				} 
			}

			refresh();
		}
};
int main() {
	//start ncurses
	initscr();
	noecho();
	printw("hello world!");

	Board b;
	b.draw_board(10, 10);
	
	//end ncurses
	refresh();
	getch();
	clear;
	endwin();
	system("clear");
	return 0;
}
