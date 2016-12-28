/*
 *  QEMU <-> ATLAS Socket APIs
 *
 *  Copyright (C) Sierra Wireless Inc.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "qemu_com.h"

/**/
/**/
int qemu_com_socket_receive(int socket, uint32_t size,uint8_t* out)
{
int r=0;
uint8_t* buf=out;
uint32_t len = size;

	while(len)
	{
		if((r = recv(socket,buf,len,0)) == -1)
		{
			if((errno == EPIPE) || (errno == EINTR))
			{
				continue;
			}
			else
			{
				fprintf(stderr, "Error Socket Recv %d %s.\n", errno, strerror(errno));
				exit(1);
			}
		}
		else
		{
			buf += r;
			len -= r;
		}
	}

return size;
}

/**/
/**/
static void qemu_com_socket_tcp_nodelay(int socket)
{
int type;
int l=sizeof(int);

    getsockopt(socket, SOL_SOCKET, SO_TYPE, &type, (socklen_t*)&l);
    if (type == SOCK_STREAM)
    {
	int flag=1;
	setsockopt(socket, IPPROTO_TCP, TCP_NODELAY,(char *)&flag, sizeof(int));
    }
}

/**/
/**/
void qemu_com_socket_send(int socket, const void* Msg, int size)
{
int r=0;
int shift=0;

	while(size)
	{
		if((r = send(socket,Msg+shift,size,0)) == -1)
		{
			if((errno == EPIPE) || (errno == EINTR))
				continue;
			else
			{
				fprintf(stderr, "Send to Tester: Error while sending over socket %d %s.\n", errno, strerror(errno));
				exit(1);
			}
		}
		else
		{
			shift += r;
			size -= r;
		}
	}
	qemu_com_socket_tcp_nodelay(socket);
}
