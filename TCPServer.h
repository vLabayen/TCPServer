#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <thread>

class TCPServer {
	public:
		static const int server_backlog = 10;

	private:
		const int server_port;
		const bool verbose;
		const std::function <void(const int, const char*, const int)> handle_connection;
		int sock = 0;
//		std::thread main_thread;

	public:
		TCPServer(const int server_port, const std::function <void(const int client_sock, const char* client_ip, const int client_port)> handle_connection, const bool verbose = false);
		~TCPServer();

	private:
		void _check(const int status, const char* error_msg);
		void _check(const int status, const char* error_msg, const char* success_msg);
		void accept_connections(void);
};

#endif
