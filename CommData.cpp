#include "CommData.hpp"



CommData::CommData()
{
}


CommData::~CommData()
{
}

void CommData::addIPtoList(string IP)
{
	mtxIPs.lock();

	IPList.insert(IP);

	mtxIPs.unlock();
}

const set<string> &  CommData::getIPList() const
{
	return IPList;
}

const vector<ClientMessages>& CommData::getClientList() const
{
	return clientMSGs;
}

void CommData::clearIPList()
{
	IPList.clear();
}

bool CommData::newClient(string IP)
{
	mtxMSGs.lock();

	ClientMessages newbe{ IP };

	int size = clientMSGs.size();
	if (size > 0) {
		for (int i = 0; i < size; i++) {
			if (clientMSGs[i].getIP() == IP) {
				mtxMSGs.unlock();
				return false;
			}
		}
	}

	clientMSGs.push_back(move(newbe));

	mtxMSGs.unlock();
	return true;
}

bool CommData::removeClient(string IP)
{
	mtxMSGs.lock();
	
	int size = clientMSGs.size();
	if (size > 0) {
		for (int i = 0; i < size; i++) {
			if (clientMSGs[i].getIP() == IP) {
				clientMSGs.erase(clientMSGs.begin() + i);

				mtxMSGs.unlock();
				return true;
			}
		}
	}

	mtxMSGs.unlock();
	return false;
}

void CommData::removeAll()
{
	mtxMSGs.lock();

	clientMSGs.clear();

	mtxMSGs.unlock();
}

void CommData::newMessage(string IP, string MSG)
{
	mtxMSGs.lock();

	int size = clientMSGs.size();
	if (size > 0) {
		for (int i = 0; i < size; i++) {
			if (clientMSGs[i].getIP() == IP) {
				clientMSGs[i].addMessage(MSG);

				break;
			}
		}
	}

	mtxMSGs.unlock();
}

const deque<string> & CommData::getClientMessages(string IP) const
{
	for (const ClientMessages &client : clientMSGs) {
		if (client.getIP() == IP) {
			return client.getMessages();
		}
	}

	return clientMSGs[0].getMessages();
}
