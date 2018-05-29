#pragma once
//#include "vld.h"
#include "Client.hpp"
#include "ClientManager.hpp"
#include "Server.hpp"
#include "UdpCom.hpp"
#include "WS_Initiator.h"
#include "CommData.hpp"
#include "IOHandling.hpp"
#include "GameTable.hpp"
#include <chrono>

using namespace std::chrono;

class Control
{
private:
	winsock _winsock;

	CommData comData;

	IOHandling io;

	Server server;
	ClientManager manager;
	UdpCom udp;

	thread acceptConThread;

	void showIPList(string errMSG, vector<string> *container = nullptr) const;

	bool chekIfClientIsActive(string IP) const;

	void showClientErr(const Client &client) const;
public:
	Control(string name);
	~Control();

	IOHandling& getIO();

	void udpBroadCast();

	void showIPs() const;

	void connectToPeer();

	void showChat();

	void disconnectionFrom();

	void peerErrs();

	void deletePeer();
};

