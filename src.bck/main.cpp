#include <stdio.h>
#include <stdlib.h>     /* atexit */
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/types.h>
#include "ServerSocket.h"
#include <errno.h>
#include <iostream>
#include <map>
#include <string>
#include "http.h"
#include "Parser.hpp"
#include "main.h"

#define BYTES 1024
#define DBG

void fnExit();
ServerSocket *OnExit;
int req = 0;

extern std::map<std::string, std::string> http_request;

int sendFile (char *fname, int flen, Socket *socket)
{
int count=0;
unsigned char buffer[BYTES];

    FILE *pFile = fopen(fname,"r");

    if ( !pFile )
    {
        printf ("Error fopen: %s\n",strerror(errno)); // get string message from errno
        return -1;
    }
    else {
        unsigned long bytes_read;

    do {
        bytes_read = fread (buffer , 1, BYTES , pFile);
        count += bytes_read;
        socket->sendStream((char *)buffer, (int)bytes_read);

    } while  ( count != flen);
    fclose (pFile);
    }
    return 0;
}

off_t Get_File_Size (char *fname)
{
    struct stat stat_buf;
    off_t rc = stat(fname, &stat_buf);
    if (rc == 0 )
        rc = stat_buf.st_size;

    return rc;
}


void  *request(void *ptr)
{
	Socket *socket = (reinterpret_cast<Socket*>(ptr));

    unsigned long len;
    int flen;
    char file_size[10];
    time_t now = time(0);

    tm *gmtm = gmtime(&now);
    char *dt = asctime(gmtm);

    int FirstLine = 1;

    std::string buff, data;
    std::string page;

    std::string uri;
    std::string content, header;

    char *path;

    char fname[256];


	while( (len = socket->readLine(buff) >0) )
	{

		if(buff.at(0) == '\r' || len == 0)
		{
			break;
		}
        if (FirstLine)
        {
            FirstLine = 0;
            parseFirstLine(buff);
        }
        else
            parseHeader(buff);
    }
    FirstLine = 1;
    path = GetPath();

    switch (GetRequestType()) {

        case GET:
        {
		printf("Sending: %s\n", path);
            if (strstr (path, "favicon.ico")) // Get the file name/type from http header
            {

                strcpy (fname, LOCAL_PATH "favicon.ico");
                flen = (int)Get_File_Size(fname);
                if ( flen == NOT_FOUND)
                {
                    strcpy (fname, LOCAL_PATH "NotFound.html");
                    flen = (int)Get_File_Size(fname);
                    header =  "HTTP/1.1 404 Not Found\r\n";
                    header += "Content-Type: text/html; charset=UTF-8\r\n";
                    header += "Content-Length: " ;
                    sprintf (file_size,"%ld",(long) flen);
                    header += file_size;
                    header += " \r\n" ;
                }
                else
                {
                    header =  "HTTP/1.1 200 OK\r\n";
                    header += "Content-Type: image/x-icon\r\n";
                    header += "Content-Length: " ;
                    sprintf (file_size,"%ld",(long) flen);
                    header += file_size;
                    header += " \r\n" ;

                }
            }
            else  // Handle text/html file
            {

                strcpy (fname, LOCAL_PATH "index.html");
                flen = (int)Get_File_Size(fname);
                if ( flen == NOT_FOUND )
                {
                    strcpy (fname, LOCAL_PATH "NotFound.html");
                    flen = (int)Get_File_Size(fname);
                    header =  "HTTP/1.1 404 Not Found\r\n";
                    header += "Content-Type: text/html; charset=UTF-8\r\n";
                    header += "Content-Length: " ;
                    sprintf (file_size,"%ld",(long) flen);
                    header += file_size;
                    header += " \r\n" ;

                }
                else {
                    header =  "HTTP/1.1 200 OK\r\n";
                    header += "Content-Type: text/html; charset=UTF-8\r\n";
                    header += "Content-Length: " ;
                    sprintf (file_size,"%ld",(long) flen);
                    header += file_size;
                    header += " \r\n" ;
                }

            }

            header += "Date: ";
            header += dt;
            header += "Server: gmwalk 0.1\r\n" \
                        "Connection: close\r\n" \
                        "\r\n";
            socket->sendStream(header.c_str(),(int)header.size());

            sendFile (fname, flen, socket);

        }
        break;

        case POST:
        {
            int count = 0;
            std::string msg;
            if ( ! (count=GetContent_Length()) )
            {
                printf ("Content-Length not found");
                break;
            }

            if ((len = socket->readData(msg, count)) == -1)
            {
                break;
            }
            printf ("POST: %s \n", msg.c_str());
            strcpy (fname, LOCAL_PATH "ReqOK.html");
            flen = (int)Get_File_Size(fname);

            header =  "HTTP/1.1 200 OK\r\n";
            header += "Content-Type: text/html; charset=UTF-8\r\n";
            header += "Content-Length: ";
            sprintf (file_size,"%ld",(long) flen);
            header += file_size;
            header += "\r\n" ;
            header += "Date: ";
            header += dt;
            header += "Server: gmwalk 0.1\r\n" \
            "Connection: close\r\n" \
            "\r\n";

            socket->sendStream(header.c_str(),(int)header.size());
            sendFile (fname, flen, socket);

        }
        break;

        case PUT:
        {
            printf ("In PUT:\n");
        }
        break;


        default:
            printf ("In default:\n");
        break;
    }



    pthread_exit(NULL);
    return nullptr;
}



int main()
{

	const char *hostname = "berry3plus.local";
	int port = 8080;
	int backlog = 1;
    pthread_t thread;


	ServerSocket *serversocket = new ServerSocket(hostname, port, backlog);
    OnExit = serversocket;

    std::atexit (fnExit);

    while(1)
	{
        int ret;
        Socket *socket = serversocket->accept();

		ret = pthread_create (&thread, NULL, request, (void*)socket);

    /* wait for the server thread to finish. */

    if(pthread_join(thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        break;
        }
    }
	return 0;
}

void fnExit()
{
    OnExit->close();
    printf ("Exit function 1.\n");
    getchar();
}

