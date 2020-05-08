battle_server: server.o game.o
	g++ server.o game.o -o battle_server -lzmq

server.o: server.cc player.h board.h game.h
	g++ -c server.cc -lzmq

game.o: game.cc game.h player.h
	g++ -c game.cc

