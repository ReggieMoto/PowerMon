/* ============================================================== */
/*
 * powermon_svc.c
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
/*
#include <stdio.h>
#include <stdlib.h>
*/
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/address.h>
#include <avahi-common/error.h>

#if USE_POLL
#include <avahi-common/watch.h>
#else
#include <avahi-common/simple-watch.h>
#endif

#include "powermon_svc.h"
#include "powermon_logger.h"
#include "common.h"

#define USE_POLL 0

typedef AvahiWatch* (*AvahiWatchNew)(const AvahiPoll *api, int fd, AvahiWatchEvent event, AvahiWatchCallback callback, void *userdata);
typedef void (*AvahiWatchUpdate)(AvahiWatch *w, AvahiWatchEvent event);
typedef AvahiWatchEvent (*AvahiWatchGetEvents)(AvahiWatch *w);
typedef void (*AvahiWatchFree)(AvahiWatch *w);

typedef AvahiTimeout* (*AvahiTimeoutNew)(const AvahiPoll *api, const struct timeval *tv, AvahiTimeoutCallback callback, void *userdata);
typedef void (*AvahiTimeoutUpdate)(AvahiTimeout *, const struct timeval *tv);
typedef void (*AvahiTimeoutFree)(AvahiTimeout *t);

static AvahiClient *avahiClient = (AvahiClient *)NULL;
static AvahiSimplePoll *simplePollApi = (AvahiSimplePoll *)NULL;
static AvahiPoll *pollApi = (AvahiPoll *)NULL;
static AvahiEntryGroup *group = (AvahiEntryGroup *)NULL;
static char svcName[] = "PowerMon";
static char svcType[] = "powermon";

static pthread_mutex_t avahi_svc_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t avahi_svc_tid = (pthread_t)NULL;

/* =================================
 * initAvahiPollApi
 */

int initAvahiPollApi(void)
{
	int rc;

	simplePollApi = avahi_simple_poll_new();

	if (simplePollApi)
	{
		pollApi = avahi_simple_poll_get(simplePollApi);

		if (!pollApi)
		{
			POWERMON_LOGGER(AVAHI, FATAL, "Unable to get the Avahi poll API.\n",0);
			avahi_simple_poll_free(simplePollApi);
	    	rc = -1;
		}
		else
			rc = 0;
	}
	else
	{
		POWERMON_LOGGER(AVAHI, FATAL, "Unable to get the Avahi simple poll.\n",0);
    	rc = -1;
	}

	return (rc);
}

/* =================================
 * CreateAvahiServices
 */

static void avahiaEntryGroupCallback(AvahiEntryGroup *avahiGroup, AvahiEntryGroupState state, AVAHI_GCC_UNUSED void *userdata)
{
    assert(avahiGroup == group || group == NULL);

    /* Called whenever the entry group state changes */
    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
            /* The entry group has been established successfully */
        	POWERMON_LOGGER(AVAHI, INFO, "Service '%s' successfully established.", svcName);
            break;
        case AVAHI_ENTRY_GROUP_COLLISION :
            /* char *n; */
            /* A service name collision with a remote service
             * happened. Let's pick a new name */
            /* n = avahi_alternative_service_name(svcName); */

        	POWERMON_LOGGER(AVAHI, WARN, "Avahi service name collision, renaming service to '%s'", svcName);
            /* And recreate the services */
            /* create_services(avahi_entry_group_get_client(g)); */
            break;
        case AVAHI_ENTRY_GROUP_FAILURE :
        	POWERMON_LOGGER(AVAHI, FATAL, "Avahi entry group failure",0);
            /* Some kind of failure happened while we were registering our services */
            avahi_simple_poll_quit(simplePollApi);
            break;
        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
        default:
            break;
    }
}

/* =================================
 * CreateAvahiServices
 */

void createAvahiServices(AvahiClient *avahiClient)
{
	if (avahiClient)
	{
		POWERMON_LOGGER(AVAHI, INFO, "Create Avahi PowerMon Service.\n", 0);

		/* If this is the first time we're called, let's create a new entry group if necessary */
		group = avahi_entry_group_new(avahiClient, avahiaEntryGroupCallback, NULL);

		if (group)
			avahi_entry_group_add_service(
					group,
					AVAHI_IF_UNSPEC,
					AVAHI_PROTO_INET,
					0,
					svcName,
					svcType,
					NULL,
					NULL,
					52955,
					NULL);
		else
			POWERMON_LOGGER(AVAHI, FATAL, "Avahi entry group creation failed: %s", avahi_strerror(avahi_client_errno(avahiClient)));
	}
	else
	{
		POWERMON_LOGGER(AVAHI, FATAL, "Avahi client invalid.\n", 0);
	}
}

/* =================================
 * avahiClientCallback
 */

void avahiClientCallback( AvahiClient *client, AvahiClientState state, void* userdata)
{
	if (client)
	{
		/* Called whenever the client or server state changes */
		switch (state)
		{
			case AVAHI_CLIENT_S_RUNNING:
				POWERMON_LOGGER(AVAHI, INFO, "Received Avahi state change (AVAHI_CLIENT_S_RUNNING).\n", 0);
				/* The server has startup successfully and registered its host
				 * name on the network, so it's time to create our services */
				createAvahiServices(client);
				break;

			case AVAHI_CLIENT_FAILURE:
				POWERMON_LOGGER(AVAHI, INFO, "Received Avahi state change (AVAHI_CLIENT_FAILURE).\n", 0);
				POWERMON_LOGGER(AVAHI, FATAL, "Client failure: %s\n",avahi_strerror(avahi_client_errno(client)));
				break;

			case AVAHI_CLIENT_S_COLLISION:
				POWERMON_LOGGER(AVAHI, INFO, "Received Avahi state change (AVAHI_CLIENT_S_COLLISION).\n", 0);
				/* Let's drop our registered services. When the server is back
				 * in AVAHI_SERVER_RUNNING state we will register them
				 * again with the new host name. */
				break;

			case AVAHI_CLIENT_S_REGISTERING:
				POWERMON_LOGGER(AVAHI, INFO, "Received Avahi state change (AVAHI_CLIENT_S_REGISTERING).\n", 0);
				/* The server records are now being established. This
				 * might be caused by a host name change. We need to wait
				 * for our own records to register until the host name is
				 * properly established. */
				if (group)
					avahi_entry_group_reset(group);
				break;

			case AVAHI_CLIENT_CONNECTING:
				POWERMON_LOGGER(AVAHI, INFO, "Received Avahi state change (AVAHI_CLIENT_CONNECTING).\n", 0);
				break;

			default:
				POWERMON_LOGGER(AVAHI, WARN, "Received Avahi state change (Unknown).\n", 0);
				break;
		}
	}
	else
		POWERMON_LOGGER(AVAHI, WARN, "Avahi callback client not valid.\n",0);
}

/* =================================
 * registerPowerMonSvc
 */

int registerPowerMonSvc(void)
{
	AvahiClientFlags flags = 0;
	int userData;
	int avahiErr, rc;

	/* Allocate main loop object */
	rc = initAvahiPollApi();

    if ((rc == 0) && pollApi)
    {
        /* Allocate a new client */
    	avahiClient = avahi_client_new( pollApi, flags, avahiClientCallback, &userData, &avahiErr);

    	if (avahiClient)
    	{
    		POWERMON_LOGGER(AVAHI, INFO, "Avahi client creation succeeded.\n",0);
        	rc = 0;
    	}
    	else
    	{
    		POWERMON_LOGGER(AVAHI, FATAL, "Avahi client creation failed.\n",0);
    		avahi_simple_poll_free(simplePollApi);
        	rc = -1;
    	}
    }
    else
    {
    	POWERMON_LOGGER(AVAHI, FATAL, "Failed to create simple poll object.\n",0);
    	rc = -1;
    }

    return (rc);
}

static unsigned int avahi_thread_active = TRUE;

void release_avahi_svc_resources(void)
{
	pthread_mutex_lock(&avahi_svc_mutex);

	if(simplePollApi)
	{
		avahi_simple_poll_quit(simplePollApi);
		avahi_simple_poll_free(simplePollApi);
		simplePollApi = (AvahiSimplePoll *)NULL;
	}

	if(avahiClient)
	{
		avahi_client_free(avahiClient);
		avahiClient = (AvahiClient *)NULL;
	}

	pthread_mutex_unlock(&avahi_svc_mutex);
}

void set_avahi_thread_inactive(void)
{
	POWERMON_LOGGER(AVAHI, DEBUG, "Signaling avahi_svc_thread to terminate.\n",0);

	avahi_thread_active = FALSE;
	release_avahi_svc_resources();
}

/* =================================
 * void avahi_thread(void)
 */
void* avahi_svc_thread(void *arg)
{
	int status;
	sem_t *sem = (sem_t *)arg;

	/* Register the powermon service with Avahi */
	status = registerPowerMonSvc();

	if (status == 0)
	{
		sem_wait(sem);
		POWERMON_LOGGER(AVAHI, THREAD, "avahi_svc_thread is alive.\n",0);

		do {

			/* Run the main loop */
			avahi_simple_poll_loop(simplePollApi);

		} while (avahi_thread_active == TRUE);

		POWERMON_LOGGER(AVAHI, THREAD, "Exiting avahi_svc_thread.\n",0);
	}

	release_avahi_svc_resources();
	pthread_exit((void *)NULL);

	return ((void *)NULL);
}

/* =================================
 * pthread_t get_avahi_svc_tid(void)
 */
pthread_t get_avahi_svc_tid(void)
{
	return (avahi_svc_tid);
}

/* =================================
 * void avahi_thread_create(void)
 */
void avahi_svc_thread_create(sem_t *sem)
{
    int err = pthread_create(
    		&avahi_svc_tid,
    		NULL,
			&avahi_svc_thread,
			(void *)sem);

    if (err != 0)
    	POWERMON_LOGGER(DSTORE, FATAL, "Unable to create avahi_svc_thread :[%s]\n", strerror(err));
}

