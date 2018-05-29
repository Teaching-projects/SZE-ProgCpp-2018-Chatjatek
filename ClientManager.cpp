#include "ClientManager.hpp"



void ClientManager::ConnectionAcceptedHandler(SOCKET& clientSocket)
{
	Client && clientToStore{ move(clientSocket), comData };

	if (comData.newClient(clientToStore.getConnectionIP())) {

		clients.push_back(move(clientToStore));

		Client & clientRef{ clients.back() };

		thread t(&Client::rcvData, &clientRef);

		rcvThreads.push_back(move(t));
	}
}

Client & ClientManager::getSpecClient(string IP)
{
	int size = clients.size();
	for (int i = 0; i < size; i++) {
		if (clients[i].getConnectionIP() == IP)
			return clients[i];
	}

	return clients.at(0);
}

const Client & ClientManager::getSpecClientConst(string IP) const
{
	int size = clients.size();
	for (const Client & client : clients) {
		if (client.getConnectionIP() == IP)
			return client;
	}

	return clients.at(0);
}

ClientManager::ClientManager(Server& pSource, CommData &comData) : pSource{ &pSource }, comData{ comData }
{
	__hook(&Server::ConnectionAccepted, this->pSource, &ClientManager::ConnectionAcceptedHandler);
}


ClientManager::~ClientManager()
{
	__unhook(&Server::ConnectionAccepted, pSource, &ClientManager::ConnectionAcceptedHandler);

	ShutDownConnections();

	for (auto& t : rcvThreads) {
		/*try {*/
			t.join();
		/*}
		catch(exception ex){}*/
	}
}

bool ClientManager::connectToPeer(string IP, int Port)
{
	Client client{comData};

	if (client.ConnectTo(IP, to_string(Port))) {

		if (comData.newClient(client.getConnectionIP())) {

			clients.push_back(move(client));

			Client & clientToReceiveAsync{ clients.back() };

			thread t(&Client::rcvData, &clientToReceiveAsync);

			rcvThreads.push_back(move(t));

			return true;
		}
	}

	return false;
}

const vector<Client>& ClientManager::getClients() const
{
	return clients;
}

const Client & ClientManager::getClient(string IP) const
{
	return getSpecClientConst(IP);
}

bool ClientManager::sendMessageAll(char * sendbuf)
{
	return false;
}

bool ClientManager::sendMessageTo(string message, string IP)
{
	Client & client = getSpecClient(IP);

	return client.sendMessage(message.c_str());
}

void ClientManager::ShutDownConnection(string IP)
{
	int size = clients.size();
	for (int i = 0; i < size; i++) {
		if (clients[i].getConnectionIP() == IP) {
			clients[i].ShutDownConnection();

			return;
		}
	}
}

void ClientManager::ShutDownConnections()
{
	for (Client& c : clients) {
		c.ShutDownConnection();
	}
}

bool ClientManager::deleteClient(string IP)
{
	int size = clients.size();
	for (int i = 0; i < size; i++) {
		if (clients[i].getConnectionIP() == IP) {
			clients[i].ShutDownConnection();
			clients.erase(clients.begin() + i);

			return true;
		}
	}

	return false;
}

void ClientManager::deleteClients()
{
	ShutDownConnections();

	clients.clear();
}
