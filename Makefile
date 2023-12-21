all: server client

server: client.cpp message_format.h game_context.h
	g++ client.cpp -o client.out
client: server.cpp message_format.h game_context.h
	g++ server.cpp -o server.out
