#include "Server.hpp"



//void Server::ClientSocketAccepted(SOCKET ClientSocket)
//{
//	cout << "Socket accepted successfully!" << endl;
//
//	int iResult = 0;
//	const int recvbuflen = 512;
//	char recvbuf[recvbuflen];
//
//	do {
//		int iSendResult;
//
//		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
//		if (iResult > 0) {
//			std::cout << "Message received from: ... Message: " << recvbuf << std::endl;
//
//			// Echo the buffer back to the sender
//			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
//			if (iSendResult == SOCKET_ERROR) {
//				printf("send failed with error: %d\n", WSAGetLastError());
//				return;
//			}
//			printf("Bytes sent: %d\n", iSendResult);
//		}
//		else if (iResult == 0)
//			printf("Connection closing...\n");
//		else {
//			printf("recv failed with error: %d\n", WSAGetLastError());
//			return;
//		}
//
//		//ShutDownConnection();
//
//	} while (iResult > 0);
//
//	shutdown(ClientSocket, SD_SEND);
//	closesocket(ClientSocket);
//}

Server::Server() : ConnectionBase(), ListenSocket{ INVALID_SOCKET }, result{ NULL }
{
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
}

Server::Server(const Server & other) : ConnectionBase(other), ListenSocket {other.ListenSocket}, result{other.result}
{
}

Server::Server(Server && other) : ConnectionBase(move(other)), ListenSocket{ INVALID_SOCKET }, result{ NULL }
{
	ListenSocket = other.ListenSocket;
	result = other.result;

	other.ListenSocket = INVALID_SOCKET;
	other.result = nullptr;
}


Server::~Server()
{
	ShutDownConnection();
}

void Server::createErr()
{
	setErrMSG("csinált error!!!!");
}

bool Server::Listen(string PORT)
{
	const char *_port = PORT.c_str();

	try {
		int iResult = getaddrinfo(NULL, _port, &hints, &result);
		if (iResult != 0)
			throw run_chain_err("getaddrinfo failed with error: " + iResult);


		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET)
			throw run_chain_err("socket failed with error: " + WSAGetLastError());


		// Setup the TCP listening socket
		iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
			throw run_chain_err("bind failed with error: " + WSAGetLastError());


		freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR)
			throw run_chain_err("listen failed with error: " + WSAGetLastError());

	}
	catch (exception ex) {
		setErrMSG(ex.what());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		return false;
	}

	return true;
}

void Server::AcceptCon()
{
	SOCKET&& ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		setErrMSG("accept failed with error: " + WSAGetLastError());
		closesocket(ListenSocket);
		return;
	}

	__raise ConnectionAccepted(ClientSocket);

	AcceptCon();
}

void Server::ShutDownConnection()
{
	shutdown(ListenSocket, SD_SEND);
	closesocket(ListenSocket);

	//WSACleanup();
}
