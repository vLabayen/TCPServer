all: server

server: server.cpp TCPServer.h TCPServer.cpp
	g++ -std=c++2a server.cpp TCPServer.h TCPServer.cpp -o server -I. -lpthread
