#include "Socket.h"
#include <sys/types.h>

using namespace std;

int Socket::m_numAccept = 0;
Socket::Socket(SOCKET socket,sockaddr_in remoteSockAddr, sockaddr_in localSockAddr, int localPort)
{
	Startup();
	m_socket = socket;
	m_socketAddress = remoteSockAddr;
	m_localSockAddr = localSockAddr;
	m_localPort = localPort;
}

Socket::Socket(const char *hostName, int port)
{
	m_hostName = hostName;
	m_localPort = port;
}

Socket::~Socket()
{
	if(m_socket != '\0')
	{
		shutdown(m_socket,1);
		closesocket(m_socket);

		m_socket='\0';
		if(m_numAccept > 0)
		{
			m_numAccept--;
		}		
	}
}

void Socket::setSocketAddress()
{
	LPHOSTENT hostEntry;
	const char *hostName = m_hostName;
	int port = m_localPort;

	if(hostName == NULL)
	{
		char host[80];

		if(gethostname(host,sizeof(host)) != SOCKET_ERROR)
		{
			hostName = host;
		}

	}

	//Now we populate the sockaddr_in structure
	memset(&m_socketAddress, 0, sizeof(m_socketAddress));	// clear struct
	m_socketAddress.sin_family=AF_INET; //Address family
	if(hostName != NULL)
	{
		hostEntry = gethostbyname(hostName);

		if(!hostEntry)
		{
			throw "could not resolve host";
		}
		if((hostEntry = gethostbyname(hostName)) != NULL)
		{

#ifdef WIN32
			struct in_addr *inet_addr = (LPIN_ADDR)*hostEntry->h_addr_list;
			m_socketAddress.sin_addr = *(inet_addr); //Wild card IP address
#else
			memcpy(&m_socketAddress.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);
#endif 
		}
	}
	else
	{
		m_socketAddress.sin_addr.s_addr = INADDR_ANY; //Wild card IP address
	}

	m_socketAddress.sin_port=htons((u_short)port); //port to use

	//the socket function creates our SOCKET
	m_socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	//If the socket() function fails we exit
	if(m_socket==INVALID_SOCKET)
	{
		throw "INVALID_SOCKET";
	}
}

SOCKET Socket::accept()
{
	SOCKET clientSocket;
	try
	{
		sockaddr_in from;
#ifdef WIN32
		int fromlen=sizeof(from);
		clientSocket=::accept(m_socket, (struct sockaddr*)&from, &fromlen);
#else
		unsigned int fromlen=sizeof(from);
		clientSocket=::accept(m_socket, (struct sockaddr*)&from, &fromlen);
#endif
		m_numAccept++;
		printf("Socket::accept() - m_numAccept=%d\n",m_numAccept);
	}
	catch(...)
	{
		printf("error\n");
	}

  return(clientSocket);
}
void Socket::Startup()
{
#ifdef WIN32
	WSADATA info;
	if(WSAStartup(MAKEWORD(2,0), &info))
	{
		throw "Could not start WSA";
	}
#endif
}
void Socket::Cleanup()
{
#ifdef WIN32
	WSACleanup();
#endif
}


void Socket::sendStream(const char *buffer,int numBytes)
{
	try
	{
		int flags = 0;

//		flags = MSG_NOSIGNAL;

		send(m_socket,buffer,numBytes,flags);
	}
	catch(...)
	{
		printf("sendStream %d\n",m_socket);
	}  
}

int Socket::readLine(string &buff)
{
	char ch;
	int len;
	int counter = -1;
	try
	{
		m_readLine.erase();
		while((len = recv(m_socket, &ch, 1 ,0)) > 0)
		{
			if((int)ch == 13) continue;
			if((int)ch == 10)
			{
				if(m_readLine.empty())
				{
					counter = 0;
					break;
				}
				buff = m_readLine;
				counter = m_readLine.length();
				break;
			}
			else
			{
				//m_readLine.push_back(ch);
				m_readLine.append(1, ch);
			}
		}
	}
	catch(...)
	{
		throw "unknown error from Socket::readLine";
	}

	return(counter);
}

int Socket::readData(string &buff, int length)
{
    char ch[1024+1];
    long counter = -1;
    long len;
    
    try
    {
        m_readLine.erase();

        while ((len = recv(m_socket, &ch, (long)length ,0)) != 0)
        {
            for (int i=0; i < len; i++)
                m_readLine.append(1, ch[i]);
            if ( len == length) break;
        }
        if(m_readLine.empty())
        {
            counter = -1;
        }
        else
        {
            buff = m_readLine;
            counter = m_readLine.length();
        }
     
    }
    catch(...)
    {
        throw "unknow error from Socket::readLine";
    }
    return((int)counter);
}
int Socket::bind()
{
	//bind links the socket we just created with the sockaddr_in 
	//structure. Basically it connects the socket with 
	//the local address and a specified port.
	//If it returns non-zero quit, as this indicates error
	return(::bind(m_socket,(sockaddr*)&m_socketAddress,sizeof(m_socketAddress)));
}


int Socket::listen(int backlog)
{
	//listen instructs the socket to listen for incoming 
	//connections from clients. The second arg is the backlog
	return(::listen(m_socket, backlog));
}


int Socket::connect()
{
	return(::connect(m_socket, (LPSOCKADDR)&m_socketAddress,sizeof(struct sockaddr)));
}


