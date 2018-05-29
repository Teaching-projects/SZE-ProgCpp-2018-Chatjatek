#pragma once
#include "ConnectionBase.hpp"
#include "CommData.hpp"
#include <fstream>
#include <sstream>

#define SENDER_STRING "10101010"
#define RECEIVER_STRING "11001100"
#define BUFLEN 9

class UdpCom :
	public ConnectionBase
{
private:
	CommData &comData;

	SOCKET SendSocket;
	SOCKET RecvSocket;

	sockaddr_in RecvAddr;

	string IP;
	string broadCast;
	unsigned short Port;

	thread * rcvThread;

	bool getIP_MASKAddr(string &ip, string &mask);

	void calculateBroadcast(const string& ip, const string& mask, string& broadcast);
	
public:
	UdpCom(CommData &comData, unsigned short Port = 27015);
	UdpCom(const UdpCom& other) = delete;
	UdpCom& operator=(const UdpCom&) = delete;
	UdpCom(UdpCom&& other) = delete;
	UdpCom& operator=(UdpCom&&) = delete;
	~UdpCom();

	bool isBroadcast();

	void refreshBroadcast();

	const string & getBroadCast();

	bool sendTo(const char* sendBuf = SENDER_STRING, string IP = "", int Port = 0);

	void rcvData();

	void ShutDownConnection();
};

