#compiler flags
CPPFLAGS =  -Wextra -Wpedantic -pedantic-errors -Wno-unused-variable -Wno-unused-parameter -std=c++17 -g -fmax-errors=1 -Wfatal-errors -D_GLIBCXX_DEBUG -fsanitize=undefined -fsanitize=address
#required libraries
CPPLIBS = -lncurses
#compiler version
CPPCOMP = g++-10

game: game.cc board.o
	${CPPCOMP} game.cc board.o ${CPPLIBS} ${CPPFLAGS} -o game

board.o: board.cc board.h
	${CPPCOMP} board.cc ${CPPLIBS} ${CPPFLAGS} -c

clean:
	rm game *.o
