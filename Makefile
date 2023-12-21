all: server client

server: client.cpp message_format.h
	g++ client.cpp message_format.h -o client.out
client: server.cpp message_format.h
	g++ server.cpp message_format.h -o server.out
