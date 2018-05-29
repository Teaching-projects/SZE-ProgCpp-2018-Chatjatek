#pragma once

#include "ConnectionBase.hpp"
#include "Exceptions.h"

class Server : public ConnectionBase
{
private:
	SOCKET ListenSocket;

	struct addrinfo *result;
	struct addrinfo hints;

	//void ClientSocketAccepted(SOCKET ClientSocket);
public:
	Server();
	Server(const Server& other);
	Server& operator=(const Server&) = delete;
	Server(Server&& other);
	Server& operator=(Server&&) = delete;
	~Server();

	void createErr();

	bool Listen(string PORT = "27015");

	void AcceptCon();

	void ShutDownConnection();

	__event void ConnectionAccepted(SOCKET &clientSocket);
};

