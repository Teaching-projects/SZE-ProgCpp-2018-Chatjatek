#pragma once

//#include "ConnectionBase.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "CommData.hpp"
#include <vector>

class ClientManager 
{
private:
	CommData &comData;

	vector<Client> clients;

	vector<thread> rcvThreads;

	Server* pSource;

	void ConnectionAcceptedHandler(SOCKET& clientSocket);

	Client & getSpecClient(string IP);

	const Client & getSpecClientConst(string IP) const;

public:
	ClientManager(Server& pSource, CommData &comData);
	~ClientManager();

	bool connectToPeer(string IP, int Port = 27015);

	const vector<Client>& getClients() const;

	const Client& getClient(string IP) const;

	bool sendMessageAll(char *sendbuf);

	bool sendMessageTo(string message, string IP);

	void ShutDownConnection(string IP);

	void ShutDownConnections();

	bool deleteClient(string IP);

	void deleteClients();
};

