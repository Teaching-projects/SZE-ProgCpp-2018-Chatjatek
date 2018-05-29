#pragma once

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <deque>
#include <mutex>

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

//#define ERR_MSG_NMB 10
//#define ERR_MSG_DEF_VAL "There is no error!"

class ConnectionBase
{
private:
	mutex* mtx;
	deque<string> errMSG;
	static constexpr int ERR_MSG_NMB{ 10 };
	static constexpr char * ERR_MSG_DEF_VAL{ "There is no error!" };
protected:
	void setErrMSG(string&& msg);
public:
	ConnectionBase();
	ConnectionBase(const ConnectionBase& other);
	ConnectionBase& operator=(const ConnectionBase&) = delete;
	ConnectionBase(ConnectionBase&& other);
	ConnectionBase& operator=(ConnectionBase&& other);
	virtual ~ConnectionBase();

	virtual void ShutDownConnection() = 0;

	const string& getLastErr() const;

	/*deque<string>::const_reverse_iterator & getErrItBegin() const;

	deque<string>::const_reverse_iterator & getErrItEnd() const;*/

	const deque<string> & getErrs() const;
};

