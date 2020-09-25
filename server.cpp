#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <chrono>
#include "TCPServer.h"

#define MAX_PACKET_LEN 1024
#define SERVER_PORT 6400

void handle_connection(const int client_sock, const char* client_ip, const int client_port) {
	char buffer[MAX_PACKET_LEN];
	while (true) {
		memset(&buffer, 0, MAX_PACKET_LEN);
		int bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);

		if (bytes_received < 0) {
			std::cout << "Error reading message from " << client_ip << ":" << client_port << std::endl;
			break;
		}
		if (bytes_received == 0) {
			std::cout << "Client " << client_ip << ":" << client_port << " disconnected" << std::endl;
			break;
		}

		std::cout << client_ip << ":" << client_port << " - " << buffer << std::endl;

		const char* response = "Here is your data";
		int bytes_sent = send(client_sock, response, sizeof(response), 0);

		if (bytes_sent < 0) {
			std::cout << "Error sending message to " << client_ip << ":" << client_port << std::endl;
			break;
		}
	}

	shutdown(client_sock, SHUT_RDWR);
}

int main() {
	TCPServer* server = new TCPServer(SERVER_PORT, handle_connection, true);
	while (true) {
		//Do other stuff
		std::this_thread::sleep_for (std::chrono::seconds(1));
	}
}

