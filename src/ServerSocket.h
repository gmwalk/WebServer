#ifndef __SERVER_SOCKET_H__
#define __SERVER_SOCKET_H__

#include "Socket.h"

class ServerSocket : public Socket
{
public:
	ServerSocket(int port, int backlog);
	ServerSocket(const char *hostname, int port, int backlog);
	void close();
	Socket *accept();
	int getLocalPort();

protected:
	char *m_localIPAddress;
	int m_port;
	//int m_backlog;
	//sockaddr_in m_socketAddress;
};

#endif //__SERVER_SOCKET_H__
