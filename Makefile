CCFLAGS =  -lncurses -Wextra -Wpedantic -pedantic-errors -Wno-unused-variable -Wno-unused-parameter -std=c++17 -g -fmax-errors=1 -Wfatal-errors -D_GLIBCXX_DEBUG -fsanitize=undefined -fsanitize=address

game: game.cc
	g++ game.cc ${CCFLAGS} -o game

clean:
	rm game *.o
