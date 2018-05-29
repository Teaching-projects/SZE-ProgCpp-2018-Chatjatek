#pragma once

#include "ConnectionBase.hpp"
#include "CommData.hpp"
#include "Exceptions.h"
#include <iostream>
#include <bitset>

#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

typedef unsigned __int16 UINT_16;

class Client : public ConnectionBase
{
private:
	CommData &comData;

	string connectionName;
	string connectionIP;

	SOCKET ConnectSocket;

	struct addrinfo *result;
	struct addrinfo hints;

	const bool itsConnectable;

	bool isActive;

	bool stopReceive;

	void getipaddr(SOCKET s);

	void receive(int bytesToRead, char * buffer, bool isDeleteOk = false);

	void getIntToCharArr(UINT_16 length, UINT_16 type, char * arr) const;

	UINT_16 getCharArrToInt(char * sendLength) const;

	enum Type { Message = 1, Name = 2 };
public:
	static string Client::ownerName;

	Client(CommData &comData);
	Client(SOCKET&& AcceptedSocket, CommData &comData);
	Client(const Client& other);
	Client& operator=(const Client&) = delete;
	Client(Client&& other);
	Client& operator=(Client&& other);
	~Client();

	const string & getConnectionIP() const;

	string getClientData() const;

	const bool IsActive() const;

	void createErr();

	bool ConnectTo(string IP = "localhost", string PORT = "27015");

	bool sendMessage(const char *sendbuf, Type type = Message);

	void rcvData();

	void ShutDownConnection();
};

