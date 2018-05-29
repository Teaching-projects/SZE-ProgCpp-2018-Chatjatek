#include "ClientMessages.hpp"



ClientMessages::ClientMessages(string IP) : IP{ IP }
{
	Messages.push_back("Connected to " + IP);
}

ClientMessages::ClientMessages(const ClientMessages & other) : IP{ other.IP }, Messages{ other.Messages }
{ }

ClientMessages::ClientMessages(ClientMessages && other) : IP{ other.IP }, Messages{ move(other.Messages) }
{
	other.IP = "";
	other.Messages = deque<string>();
}

ClientMessages & ClientMessages::operator=(ClientMessages && other)
{
	if (this != &other) {
		IP = other.IP;
		Messages = move(other.Messages);

		other.IP = "";
		other.Messages = deque<string>();
	}

	return *this;
}

void ClientMessages::addMessage(string message)
{
	Messages.push_back(message);

	if (Messages.size() > MAX_MESSAGE_NMB)
		Messages.pop_front();
}

const string & ClientMessages::getIP() const
{
	return IP;
}

const deque<string>& ClientMessages::getMessages() const
{
	return Messages;
}
