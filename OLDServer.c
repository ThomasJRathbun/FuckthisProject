#include "Server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "mergesort.h"

pthread_mutex_t lock;


typedef struct _info{
	char* port;
}info;

#define BACKLOG 15 //max number of connections

void* newConnection(void* info)
{
	int socket = (int)(*info);
	pthread_mutex_unlock(&lock);
	node* head;
	node* data;
	char* buffer;
	int len=0;
	while( buffer != EOF)
	{
		ioctl(socket,FIONREAD, &len);
		read(socket, buffer,len);

	}


	//read into Nodes
	//sort
	//write to socket
	//close socket
	return 0;
}

int acceptConnections( void* args)
{
	struct addrinfo* hostinfo;
	struct addrinfo hints = NULL;
	struct sockaddr clientAddr;
	char* port = ((info)args)->port;
	int listenSock, clientSock;

	pthread_t tid;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;


	if( getaddrinfo(NULL, port, &hints, &hostinfo) != 0)
	{
		printf("Can't create host\n");
		return -1;
	}

	listenSock = socket(hostinfo->ai_family, hostinfo->ai_socktype, hostinfo->ai_protocol);

	if ( bind(listenSock, hostinfo->ai_addr, hostinfo->ai_addrlen) !=0)
		{
		printf("could not bind\n");
			return -2;
		}

	if ( listen(listenSock, BACKLOG) != 0 )
	{
		printf("could not listen\n");
		return -3;
	}

	while (1)
	{
		socklen_t clientLen = sizeof(clientAddr);
		pthread_mutex_lock(&lock);
		if ( (clientSock = accept(listenSock,&clientAddr, &clientLen  )) != 0)
		{
			printf("can not accept\n");
			return -4;
		}
		else
		{
			if( pthread_create( &tid, NULL, &clientAddr, &clientSock) != 0 )
			{
				printf("could not create thread\n");
				return -5;
			}

		}
	}




	return 0;
}
