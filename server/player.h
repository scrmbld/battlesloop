#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "board.h"
using std::string;

struct Player {
	string uname;
	std::vector<std::vector<char>> board{0, std::vector<char>(0, '~')};
};
