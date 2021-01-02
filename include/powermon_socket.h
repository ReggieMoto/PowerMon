/* ============================================================== */
/*
 * powermon_socket.h
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

#ifndef __POWERMON_SOCKET_H__
#define __POWERMON_SOCKET_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "common.h"

typedef int SOCKET_SD;
typedef struct sockaddr_in SOCKADDRIN;
typedef struct sockaddr SOCKADDR;

#define INVALID_SOCKET -1
#define MAXPENDING 20    /* Max connection requests */

typedef struct ip_addr {
  union {
    struct {
      u_char s_b1,s_b2,s_b3,s_b4;
    } S_un_b;
    u_long S_addr;
  } S_un;
} IP_ADDR;

int createSocket(void);
int connectSocket(void);
bool wifiSocketIsActive(void);
bool wifiSocketIsNotActive(void);
int svcSocketReceive(unsigned int *, char *, int );
void svcSocketSend(unsigned int, char *, int );

#endif /* __POWERMON_SOCKET_H__ */
