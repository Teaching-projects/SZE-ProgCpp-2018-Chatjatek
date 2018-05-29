#include "Client.hpp"



void Client::getipaddr(SOCKET s)
{
	sockaddr_in SockAddr;
	int addrlen = sizeof(SockAddr);

	if (getsockname(s, (LPSOCKADDR)&SockAddr, &addrlen) == SOCKET_ERROR)
	{
		setErrMSG("getipaddr: " + WSAGetLastError());
		return;
	}

	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(SockAddr.sin_addr), str, INET_ADDRSTRLEN);

	connectionIP = string(str);
}

void Client::receive(int bytesToRead, char * buffer, bool isDeleteOk)
{
	int iResult;
	int remainingBytes = 0;
	//char * buffer = new char[bytesToRead];

	while ((remainingBytes < bytesToRead) && !stopReceive) {
		iResult = recv(ConnectSocket, buffer, bytesToRead, 0);
		if (iResult > 0)
			remainingBytes += iResult;
		else if (iResult == 0) {
			if (isDeleteOk)
				delete[] buffer;
			throw run_chain_err("Connection Closed!");
		}
		else {
			if (isDeleteOk)
				delete[] buffer;
			throw run_chain_err("recv failed with error: " + WSAGetLastError());
		}
	}
}

void Client::getIntToCharArr(UINT_16 length, UINT_16 type, char * arr) const
{
	arr[0] = (length >> 8) & 0xFF;
	arr[1] = length & 0xFF;

	arr[2] = (type >> 8) & 0xFF;
	arr[3] = type & 0xFF;
}

UINT_16 Client::getCharArrToInt(char * arr) const
{
	UINT_16 rv = 0;
	int i = 15;

	auto count = [&rv, &i](string bits) {
		for (char c : bits) {
			if (c == '1')
				rv += pow(2, i);
			i--;
		}
	};

	count(bitset<8>(*arr).to_string());
	count(bitset<8>(*(arr + 1)).to_string());

	return rv;
}

Client::Client(CommData &comData) : ConnectionBase(), comData{ comData }, ConnectSocket{ INVALID_SOCKET }, result{ NULL }, itsConnectable{ true }, isActive{ false }, connectionName{ "???" }, connectionIP{ "" }, stopReceive{ false }
{
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

Client::Client(SOCKET&& AcceptedSocket, CommData &comData) : ConnectionBase{}, comData{ comData }, ConnectSocket{ AcceptedSocket }, result{ NULL }, itsConnectable{ false }, isActive{ false }, connectionName{ "???" }, connectionIP{ "" }, stopReceive{ false }
{
	getipaddr(ConnectSocket);
	if (connectionIP != "")
		isActive = true;
}

Client::Client(const Client & other) : ConnectionBase(other), comData{ other.comData }, ConnectSocket{other.ConnectSocket}, result{ other.result }, itsConnectable{other.itsConnectable}, isActive{ other.isActive }, connectionName{ other.connectionName }, connectionIP{ other.connectionIP }, stopReceive{ other.stopReceive }
{
}

Client::Client(Client && other) : ConnectionBase(move(other)), comData{ other.comData }, ConnectSocket{ other.ConnectSocket }, result{ other.result }, itsConnectable{ other.itsConnectable }, isActive{ other.isActive }, connectionName{ move(other.connectionName) }, connectionIP{ move(other.connectionIP) }, stopReceive{ other.stopReceive }
{
	other.ConnectSocket = INVALID_SOCKET;
	other.result = nullptr;
	other.connectionName = "";
}

Client & Client::operator=(Client && other)
{
	if (this != &other) {
		
		//swap(comData, comData);
		swap(ConnectSocket, other.ConnectSocket);
		swap(result, other.result);
		//swap(itsConnectable, other.itsConnectable);
		swap(isActive, other.isActive);
		swap(connectionName, other.connectionName);
		swap(connectionIP, other.connectionIP);
		swap(stopReceive, other.stopReceive);
	}

	return static_cast<Client&>(ConnectionBase::operator=(std::move(other)));
}

Client::~Client()
{
	ShutDownConnection();
}

const string & Client::getConnectionIP() const
{
	return connectionIP;
}

string Client::getClientData() const
{
	return connectionIP + " " + connectionName;
}

const bool Client::IsActive() const
{
	return isActive;
}

void Client::createErr()
{
	setErrMSG("csinált error!!!!");
}

bool Client::ConnectTo(string IP, string PORT)
{
	if (!itsConnectable) {
		setErrMSG("Its not a connectable socket!");
		return false;
	}

	const char *_IP = IP.c_str();
	const char *_port = PORT.c_str();

	try {
		int iResult = getaddrinfo(_IP, _port, &hints, &result);
		if (iResult != 0)
			throw run_chain_err("getaddrinfo failed with error: " + iResult);

		addrinfo *ptr;

		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET)
				throw run_chain_err("socket failed with error: " + WSAGetLastError());

			// Connect to server.
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET)
			throw run_chain_err("Unable to connect to server!\n");

	}
	catch (run_chain_err ex) {
		setErrMSG(ex.what());
		//WSACleanup();
		closesocket(ConnectSocket);
		return false;
	}

	sendMessage(ownerName.c_str(), Name);

	connectionIP = IP;

	isActive = true;

	return true;
}

bool Client::sendMessage(const char *sendbuf, Type type)
{
	int iResult;
	UINT_16 length = (UINT_16)strlen(sendbuf);

	char toSend[4];

	getIntToCharArr(length, type, &toSend[0]);

	iResult = send(ConnectSocket, toSend, 4, 0);
	if (iResult != SOCKET_ERROR) {
		iResult = send(ConnectSocket, sendbuf, length, 0);
		if (iResult != SOCKET_ERROR) {
			if (type == Message) {
				//std::cout << "To:<" << connectionName << ">: " << sendbuf << std::endl;
				comData.newMessage(connectionIP, "To:<" + connectionName + ">: " + sendbuf);
			}
			return true;
		}
	}

	setErrMSG({ "send failed with error: " + WSAGetLastError() });
	ShutDownConnection();
	return false;
}

void Client::rcvData()
{
	try {
		while (!stopReceive) {

			char buffer[2];

			receive(2, buffer);
			UINT_16 length = getCharArrToInt(buffer);
			receive(2, buffer);
			UINT_16 type = getCharArrToInt(buffer);

			char * msg = new char[length + 1];
			receive(length, msg, true);
			*(msg + length) = '\0';

			switch (type) {
			case Message: {
				//std::cout << "From:<" << connectionName << ">: " << msg << "     length: " << length << std::endl;
				comData.newMessage(connectionIP, "From:<" + connectionName + ">: " + msg);
				break;
			}
			case Name:
				if (connectionName != msg) {
					connectionName = msg;
					sendMessage(ownerName.c_str(), Name);
				}
				break;
			}

			delete[] msg;
		}

	}
	catch (exception ex) {
		setErrMSG(ex.what());
		isActive = false;
		printf(this->connectionIP.c_str());
		printf(this->connectionName.c_str());
	}

}

void Client::ShutDownConnection()
{
	shutdown(ConnectSocket, SD_SEND);
	closesocket(ConnectSocket);
	stopReceive = true;
	isActive = false;
}
