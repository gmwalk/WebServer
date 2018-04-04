#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "ServerSocket.h"

ServerSocket::ServerSocket(int port, int backlog) : Socket((const char*)NULL, port)
{
	Startup();

	setSocketAddress();	

	m_port = port;

	if(bind() != 0)
	{
		throw "ERROR: ServerSocket::ServerSocket() - bind";
	}

	if(listen(backlog) != 0)
	{
		throw "ERROR: ServerSocket::ServerSocket() - listen";
	}
}

ServerSocket::ServerSocket(const char *hostname, int port, int backlog) : Socket(hostname, port)
{
    int rc;
	Startup();

	setSocketAddress();	

	m_port = port;

	if((rc = bind()) != 0)
    {
        printf ("Bind error %x\n",rc);
        printf ("Error bind: %s\n",strerror(errno)); // get string message from errno
        throw "ERROR: ServerSocket::ServerSocket() - bind";
    }

	if(listen(backlog) != 0)
	{
		throw "ERROR: ServerSocket::ServerSocket() - listen";
	}
}

void ServerSocket::close()
{
	Cleanup();
	delete this;
}

Socket *ServerSocket::accept()
{
	Socket *clientSocket = NULL;
	SOCKET sock;
	try
	{
		sockaddr_in from;
#ifdef _WIN32
		int fromlen=sizeof(from);
		sock=::accept(m_socket, (struct sockaddr*)&from, &fromlen);
#else
		unsigned int fromlen=sizeof(from);
		sock=::accept(m_socket, (struct sockaddr*)&from, &fromlen);
#endif
		clientSocket = new Socket(sock,from,m_socketAddress,m_port);
		m_numAccept++;
//		printf("ServerSocket::accept() - m_numAccept=%d\n",m_numAccept);
	}
	catch(...)
	{
		throw "ERROR: ServerSocket::ServerSocket() - Unexpected Exception";
	}

  return(clientSocket);
}
int ServerSocket::getLocalPort()
{
	return m_port;
}
