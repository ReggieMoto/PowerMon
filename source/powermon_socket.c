/* ============================================================== */
/*
 * powermon_socket.c
 *
 * Copyright (c) 2017 David Hammond
 * All Rights Reserved.
 */
/* ============================================================== */
/*
 * NOTICE:  All information contained herein is and remains the
 * property of David Hammond. The intellectual and technical
 * concepts contained herein are proprietary to David Hammond
 * and may be covered by U.S.and Foreign Patents, patents in
 * process, and are protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this
 * material is strictly forbidden unless prior written permission
 * is obtained David Hammond.
 */
/* ============================================================== */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include "powermon_socket.h"
#include "powermon_logger.h"

#define SVC_PORT 52955

static bool socketIsActive = FALSE;
static SOCKET_SD srvrSocket;
/*static SOCKET_SD clntSocket; */
static SOCKADDRIN svcServer;

void setWifiSocketIsActive(void);
void setWifiSocketIsNotActive(void);

/* ========================================*/
/*
 *  createSocket
 */
/* ========================================*/
int createSocket(void)
{
	int rc;

	// Create the UDP socket for connecting to server
	srvrSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (srvrSocket == INVALID_SOCKET)
	{
		POWERMON_LOGGER(DEV_IO, FATAL, "Error creating socket.\n",0);
		setWifiSocketIsNotActive();
		rc = FAILURE;
	}
	else
	{
		POWERMON_LOGGER(DEV_IO, DEBUG, "UDP socket %d created.\n",srvrSocket);
		rc = SUCCESS;
	}

	return rc;
}

/* ========================================*/
/*
 *  connectSocket
 */
/* ========================================*/
int connectSocket(void)
{
	int sockError, rc;

	memset(&svcServer, 0,sizeof(SOCKADDRIN));
	svcServer.sin_family = AF_INET;
	svcServer.sin_addr.s_addr = htonl(INADDR_ANY);
	svcServer.sin_port = htons(SVC_PORT);

	/* Bind the server socket */
	rc = bind(srvrSocket, (SOCKADDR *) &svcServer, sizeof(SOCKADDRIN));

	if (rc == SUCCESS)
	{
			POWERMON_LOGGER(DEV_IO, INFO, "UDP socket %d bound to port %d.\n",srvrSocket, SVC_PORT);
			setWifiSocketIsActive();
	}
	else
	{
		sockError = errno;
		POWERMON_LOGGER(DEV_IO, FATAL, "UDP socket failed to bind (errno: %d).\n", sockError);
		setWifiSocketIsNotActive();
	}

	return rc;
}

/* ========================================*/
/*
 *  svcSocketReceive
 */
/* ========================================*/
int svcSocketReceive(unsigned int *nodeIp, char *buffer, int bufferLen)
{
	int msgLen, sockError;
	SOCKADDR sockAddr;
	unsigned int srcAddrLen = SRC_ADDR_LEN;

	if (wifiSocketIsActive())
	{
		msgLen = recvfrom(srvrSocket, buffer, bufferLen, 0, &sockAddr, &srcAddrLen);

		if (msgLen < 0)
		{
			sockError = errno;
			POWERMON_LOGGER(DEV_IO, INFO, "UDP socket returned an error (errno: %d).\n", sockError);
		}
		else
		{
			struct sockaddr_in *sa_in = (struct sockaddr_in *)&sockAddr;
			struct in_addr *sinAddr = &(sa_in->sin_addr);
			IP_ADDR *ipAddr = (IP_ADDR *)&(sinAddr->s_addr);
			*nodeIp = (unsigned int)(ipAddr->S_un.S_addr);

			POWERMON_LOGGER(DEV_IO, INFO, "UDP socket returned a packet of length %d.\n", msgLen);
			POWERMON_LOGGER(DEV_IO, INFO, "Packet source: %d.%d.%d.%d.\n",
					ipAddr->S_un.S_un_b.s_b1, ipAddr->S_un.S_un_b.s_b2,
					ipAddr->S_un.S_un_b.s_b3, ipAddr->S_un.S_un_b.s_b4);
		}
	}
	else
		msgLen = FAILURE;

	return msgLen;
}

/* ========================================*/
/*
 *  svcSocketSend
 */
/* ========================================*/
void svcSocketSend(unsigned int nodeIp, char *buffer, int bufferLen)
{
	struct sockaddr_in sockAddr;

	if (wifiSocketIsActive())
	{
		POWERMON_LOGGER(DEV_IO, INFO, "Request to send a packet.\n", 0);

		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(SVC_PORT);
		sockAddr.sin_addr.s_addr = nodeIp;

		int sendErr = sendto(srvrSocket, buffer, bufferLen, 0, (SOCKADDR *)&sockAddr, sizeof(SOCKADDR));

		if (sendErr < 0)
		{
			int rc = errno;
			POWERMON_LOGGER(DEV_IO, WARN, "sendto failure. (errno: %d).\n", rc);
		}
	}
	else
		POWERMON_LOGGER(DEV_IO, WARN, "WiFi socket is not active.\n",0);
}

/* ========================================*/
/*
 *  Get/Set Accessors: Socket is/is not active
 */
/* ========================================*/
bool wifiSocketIsActive(void)
{
	return (socketIsActive == TRUE);
}

bool wifiSocketIsNotActive(void)
{
	return (socketIsActive == FALSE);
}

void setWifiSocketIsNotActive(void)
{
	socketIsActive = FALSE;
}

void setWifiSocketIsActive(void)
{
	socketIsActive = TRUE;
}

