#pragma once
#include "ClientMessages.hpp"
#include <string>
#include <vector>
#include <set>
#include <mutex>

using namespace std;

class CommData
{
private:
	set<string> IPList;

	vector<ClientMessages> clientMSGs;

	mutex mtxIPs;
	mutex mtxMSGs;
public:
	CommData();
	CommData(const CommData& other) = delete;
	CommData& operator=(const CommData&) = delete;
	CommData(CommData&& other) = delete;
	CommData& operator=(CommData&&) = delete;
	~CommData();

	void addIPtoList(string IP);

	const set<string> & getIPList() const;

	const vector<ClientMessages> & getClientList() const;

	void clearIPList();

	bool newClient(string IP);

	bool removeClient(string IP);

	void removeAll();

	void newMessage(string IP, string MSG);

	const deque<string> & getClientMessages(string IP) const;
};

