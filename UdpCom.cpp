#include "UdpCom.hpp"



bool UdpCom::getIP_MASKAddr(string &ip, string &mask)
{
	try {
		FILE *stream;
		if (freopen_s(&stream, "ipconfig.txt", "w", stdout) != 0)
			exit(-1);
		system("ipconfig");
		freopen_s(&stream, "CON", "w", stdout);



		ifstream infile("ipconfig.txt");
		string line;
		bool maskLine = false;
		while (getline(infile, line))
		{
			istringstream iss(line);

			if (maskLine) {
				if (line.substr(0, 15).find("Subnet Mask") != string::npos) {
					mask = line.substr((line.find(':') + 2));

					return true;
				}

				break;
			}

			if (line.substr(0, 15).find("IPv4 Address") != string::npos) {
				ip = line.substr((line.find(':') + 2));

				maskLine = true;
			}

		}
	}
	catch (exception ex) {
		setErrMSG(ex.what());
	}

	return false;
}

void UdpCom::calculateBroadcast(const string & ip, const string & mask, string & broadcast)
{
	int i = 0;
	int length = mask.length();
	string substr = "";
	int pos = 0;

	for (i = 0; i < length; i++) {
		if (mask[i] == '.') {

			if (substr[0] != '0') {
				substr = "";
				pos++;
			}
			else {
				break;
			}

		}
		else {
			substr += mask[i];
		}
	}
	int dots = 0;
	length = ip.length();

	for (i = 0; i < length; i++) {
		if (ip[i] == '.') {
			dots++;
		}
		if (dots == pos) {
			broadcast = ip.substr(0, i);

			for (int j = 0; j < 4 - pos; j++) {
				broadcast += ".255";
			}

			break;
		}
	}
}

UdpCom::UdpCom(CommData &comData, unsigned short Port) : comData{ comData }, Port{ 27015 }, broadCast{ "" }, IP{ "" }
{
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) 
		setErrMSG("socket failed with error: " + WSAGetLastError());
	
	RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (RecvSocket == INVALID_SOCKET) 
		setErrMSG("socket failed with error: " + WSAGetLastError());
	else {
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(Port);
		RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		int iResult = ::bind(RecvSocket, (SOCKADDR *)& RecvAddr, sizeof(RecvAddr));
		if (iResult != 0) 
			setErrMSG("bind failed with error: " + WSAGetLastError());
	}

	rcvThread = new thread(&UdpCom::rcvData, this);

	string tmpMask;
	if(getIP_MASKAddr(IP, tmpMask))
		calculateBroadcast(IP, tmpMask, broadCast);

}

UdpCom::~UdpCom()
{
	ShutDownConnection();

	rcvThread->join();

	delete rcvThread;
}

bool UdpCom::isBroadcast()
{
	if (broadCast != "")
		return true;

	return false;
}

void UdpCom::refreshBroadcast()
{
	string tmpMask;
	if (getIP_MASKAddr(IP, tmpMask))
		calculateBroadcast(IP, tmpMask, broadCast);
}

const string & UdpCom::getBroadCast()
{
	return broadCast;
}

bool UdpCom::sendTo(const char* sendBuf, string IP, int Port)
{
	Port = Port == 0 ? this->Port : Port;

	IP = IP == "" ? broadCast : IP;

	sockaddr_in SendToAddr;

	SendToAddr.sin_family = AF_INET;
	SendToAddr.sin_port = htons(Port);
	//SendToAddr.sin_addr.s_addr = inet_addr(IP.c_str());
	InetPton(AF_INET, IP.c_str(), &SendToAddr.sin_addr.s_addr);		

	int iResult = sendto(SendSocket, sendBuf, BUFLEN, 0, (SOCKADDR *)& SendToAddr, sizeof(SendToAddr));			//kell mindenképp check elõtte h megvane a saját ip...
	if (iResult == SOCKET_ERROR) {
		setErrMSG("sendto failed with error: " + WSAGetLastError());
		return false;
	}

	return true;
}

void UdpCom::rcvData()
{
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);

	while (true) {

		char RecvBuf[BUFLEN];

		int iResult = recvfrom(RecvSocket, RecvBuf, BUFLEN, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			setErrMSG("recvfrom failed with error: " + WSAGetLastError());
			return;
		}

		if (strcmp(RecvBuf, SENDER_STRING) == 0) {

			//ide kell feltétel h sajátra ne reagáljon
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(SenderAddr.sin_addr), str, INET_ADDRSTRLEN);
			string IP(str);
			if (IP != this->IP) {
				comData.addIPtoList(str);

				sendTo(RECEIVER_STRING, str);
			}
		}

		if (strcmp(RecvBuf, RECEIVER_STRING) == 0) {
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(SenderAddr.sin_addr), str, INET_ADDRSTRLEN);
			comData.addIPtoList(str);
		}
	}
}

void UdpCom::ShutDownConnection()
{
	closesocket(SendSocket);

	closesocket(RecvSocket);
}
