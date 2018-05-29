#pragma once
#include "Exceptions.h"
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

struct winsock {
	static constexpr int version = 2;
	WSADATA wsaData;

	winsock() {
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (LOBYTE(wsaData.wVersion) != version || HIBYTE(wsaData.wVersion) != version)
			throw bad_version_err("Could not initialize WinSock 2.2");
	}
	~winsock() {
		WSACleanup();
	}
};
