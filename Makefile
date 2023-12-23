FLAGS = -std=c++11
all: server client

server: client.cpp socket_io.cpp message_format.h game_context.h socket_io.h
	g++ ${FLAGS} client.cpp socket_io.cpp -o client.out
client: server.cpp socket_io.cpp message_format.h game_context.h socket_io.h
	g++ ${FLAGS} server.cpp socket_io.cpp -o server.out
