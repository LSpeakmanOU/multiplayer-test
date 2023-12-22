FLAGS = -std=c++11
all: server client

server: client.cpp message_format.h game_context.h
	g++ ${FLAGS} client.cpp -o client.out
client: server.cpp message_format.h game_context.h
	g++ ${FLAGS} server.cpp -o server.out
