#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef WIN32
   #include <winsock.h>
#else
   #include <sys/socket.h>
   #include <sys/un.h>
   #include <netinet/in.h>
   #include <unistd.h>
   #include <netdb.h>

   #define SOCKET int
   #define SOCKET_ERROR -1
   #define INVALID_SOCKET -1
   #define LPSOCKADDR struct sockaddr *
   #define LPHOSTENT struct hostent *
   #define closesocket close
#endif
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#pragma comment( lib, "wsock32" )	// link with wsock32.lib
#endif

class Socket
{
public:
	Socket(SOCKET socket,sockaddr_in remoteSockAddr, sockaddr_in localSockAddr, int localPort);
	Socket(const char *hostName, int port);
	~Socket();
	void setSocketAddress();
	SOCKET accept();
	static void Startup();
	static void Cleanup();
	void sendStream(const char*,int);
	int readLine(std::string &buffer);
    int readData(std::string &buff, int length);
	int bind();
	int listen(int backlog);
	int connect();
protected:
	SOCKET m_socket;
	static int m_numAccept;
	//sockaddr_in m_local;
	char *m_localIPAddress;
	char *m_clientIPAddress;

	//The sockaddr_in specifies the address of the socket
	//for TCP/IP sockets. Other protocols use similar structures.
	sockaddr_in m_socketAddress;
	//sockaddr_in m_remoteSockAddr;
	sockaddr_in m_localSockAddr;

	int m_localPort;
	//int m_backlog;
	struct hostent *m_hp;
	const char *m_hostName;
	//const char *m_serverName;
	std::string m_readLine;
};

#endif //__SOCKET_H__
