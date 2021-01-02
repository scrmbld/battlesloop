#include <iostream>
#include <utility>
#include <vector>
#include <curses.h>

class Board {
	private:
		const int SIZE = 10;
		const std::vector<char> visual = {'~', 'S', 'M', 'H'};//maps each possible value for a position on the board to its visual representation in the game
		std::vector<std::vector<int>> vec; 
		//board's position on the screen
		const int start_x, start_y;
		//board's name (printed at start_x, start_y - 1)
		const std::string name;
	public:
		Board(int y, int x, std::string s) : start_y(y), start_x(x), name(s) {
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
		void drawBoard() {
			mvprintw(start_y - 1, start_x, name.c_str());
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					mvaddch(i + start_y, j * 2 + start_x, visual.at(vec.at(i).at(j)));
				} 
			}

			refresh();
		}

		std::pair<int, int> selectTile() {
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

};
int main() {
	//start ncurses
	initscr();
	noecho();
	printw("hello world!");

	//create boards
	const int p1_y = 10, p1_x = 5, p2_y = 10, p2_x = 40;
	Board p1(p1_y, p1_x, "You");
	Board p2(p2_y, p2_x, "Enemy");

	p1.drawBoard();
	p2.drawBoard();

	std::pair<int, int> selected;
	selected = p1.selectTile();
	mvprintw(p1_y + 11, p1_x, "%i, %i", selected.first, selected.second);
	refresh();
	getch();

	//end ncurses
	refresh();
	getch();
	clear;
	endwin();
	system("clear");
	return 0;
}
