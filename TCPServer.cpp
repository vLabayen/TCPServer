#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "TCPServer.h"

TCPServer::TCPServer(const int sp, const std::function <void(const int client_sock, const char* client_ip, const int client_port)> hc, const bool v)
	: server_port(sp), handle_connection(hc), verbose(v)
{
	if (this->verbose) std::cout << "Starting server..." << std::endl;

	//Create the socket
	this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	this->_check(this->sock, "Socket creation failed", "Socket successfully created");

	//Bind the socket
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(this->server_port);

	int bind_status = bind(this->sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
	this->_check(bind_status, "Socket bind failed", "Socket successfully binded");

	//Open port for listening
	int listen_status = listen(this->sock, TCPServer::server_backlog);
	this->_check(listen_status, "Socket listen failed");
	if (this->verbose) std::cout << "Socket listening on port " << this->server_port << std::endl;

	//Run a thread to accept incoming connections
	//https://stackoverflow.com/questions/4581476/using-boost-thread-and-a-non-static-class-function
	std::thread t(&TCPServer::accept_connections, this);
	t.detach();
}

TCPServer::~TCPServer(void)
{
	if (this->sock > 0) close(this->sock);
}

void TCPServer::_check(const int status, const char* error_msg)
{
	if (status < 0) throw std::runtime_error(error_msg);
}

void TCPServer::_check(const int status, const char* error_msg, const char* success_msg)
{
	if (status < 0) throw std::runtime_error(error_msg);
	if (this->verbose) std::cout << success_msg << std::endl;
}

void TCPServer::accept_connections(void) {
	while (true) {
		//Prepare client addr struct
		struct sockaddr_in cliaddr;
		socklen_t cliaddr_len = sizeof(cliaddr);
		memset(&cliaddr, 0, cliaddr_len);

		//Accept connection
		const int client_sock = accept(this->sock, (struct sockaddr*)&cliaddr, &cliaddr_len);

		//Obtain client addr & port
		const char* client_ip = inet_ntoa(cliaddr.sin_addr);
		const int client_port = ntohs(cliaddr.sin_port);
		if (this->verbose) std::cout << "Accepted new client " << client_ip << ":" << client_port << std::endl;

		//Handle connection
		std::thread t(&TCPServer::handle_connection, this, client_sock, client_ip, client_port);
		t.detach();
	}
}
