#pragma once
#include <string>
#include <deque>

#define MAX_MESSAGE_NMB 100

using namespace std;

class ClientMessages
{
private:
	string IP;

	deque<string> Messages;
public:
	ClientMessages(string IP);
	ClientMessages(const ClientMessages& other);
	//ClientMessages& operator=(const ClientMessages&) = delete;
	ClientMessages(ClientMessages&& other);
	ClientMessages& operator=(ClientMessages&& other);

	void addMessage(string message);

	const string & getIP() const;

	const deque<string> & getMessages() const;
};

