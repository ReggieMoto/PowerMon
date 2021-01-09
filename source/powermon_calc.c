/* ============================================================== */
/*
 * powermon_calc.c
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

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "powermon_calc.h"
#include "powermon_curses.h"
#include "powermon_time.h"
#include "device_io_data.h"
#include "powermon_pkt.h"
#include "powermon_logger.h"
#include "powermon_socket.h"

/* The active nodes sending data */
static ActiveNodeArray activeNodes;

/* One of these arrays is active and one is being processed. */
static PollCycleData pollCycleData[MAX_POLL_CYCLE_DATA_ARRAYS];
static unsigned int activeDataArray = 0; /* Index to active array */
static pthread_mutex_t pwrmonActiveDataArrayMutex = PTHREAD_MUTEX_INITIALIZER;

/* System status data */
static PwrCalcSystemStatus sysStatus;

/* Forward declaration(s) */
static ActiveNode * findActiveNode(Packet *packet);


/* ========================================*/
/*
 *  consoleReportActiveNodes
 *  consolePadOutputLine
 */
/* ========================================*/

static const char *padStrings[] = {
		"",		/* 0 */
		" ",	/* 1 */
		"  ",	/* 2 */
		"   ",	/* 3 */
		"    ",	/* 4 */
		"     ",	/* 5 */
		"      ",	/* 6 */
		"       ",	/* 7 */
		"        ",	/* 8 */
		"         ",	/* 9 */
		"          "	/* 10 */
};

/* Constants used by consoleReportActiveNodes */
#define CONSOLE_MAX_SER_NUM_LEN 22
#define CONSOLE_MAX_IP_ADDR_LEN 18
#define CONSOLE_MAX_DISABLE_LEN 10
#define CONSOLE_MAX_AVG_ON_LEN   9
#define CONSOLE_MAX_AVG_OFF_LEN  9
#define CONSOLE_MAX_AVG_AMP_LEN  5
#define CONSOLE_MAX_HRTBEAT_LEN  7
#define CONSOLE_EXTRA_LEN 20
#define CONSOLE_WORKING_LEN 32

#define CONSOLE_MAX_BUF_LEN \
		(CONSOLE_MAX_SER_NUM_LEN+CONSOLE_MAX_IP_ADDR_LEN+CONSOLE_MAX_DISABLE_LEN+CONSOLE_MAX_AVG_ON_LEN+ \
		CONSOLE_MAX_AVG_OFF_LEN+CONSOLE_MAX_AVG_AMP_LEN+CONSOLE_MAX_HRTBEAT_LEN+CONSOLE_EXTRA_LEN)

#define AMPS 2
#define VOLTS 110
#define KW (AMPS x VOLTS / 1000)f

#if 1
#define LEARNING_CYCLE_DAYS 3
#else
#define LEARNING_CYCLE_DAYS 7
#endif

#define MAX_QUIET_ALERT 60
#if 1
#define MAX_QUIET_REMOVE (60 * 2)		/* DEBUG: 60 mins x 2 hrs */
#else
#define MAX_QUIET_REMOVE (60 * 24 * 3)	/* 60 mins x 24 hrs x 3 days */
#endif

#define CALC_INTERVAL 60

void consoleCenterOutputLine(char *destBuf, char *srcBuf, int *prePadCount, int *postPadCount)
{
	assert(destBuf && srcBuf && prePadCount && postPadCount);
	*postPadCount = *prePadCount/2;
	*prePadCount -= *postPadCount;
	assert((*prePadCount>=0) && (*prePadCount<10));
	assert((*postPadCount>=0) && (*postPadCount<10));
	assert((strlen(srcBuf)+*prePadCount+*postPadCount) < CONSOLE_MAX_BUF_LEN);
	strcat(destBuf, padStrings[*prePadCount]);
	strcat(destBuf, srcBuf);
	strcat(destBuf, padStrings[*postPadCount]);
	strcat(destBuf, "|");
}

/* ========================================*/
/*
 *  consoleReportActiveNodes
 */
/* ========================================*/
void consoleReportActiveNodes(void)
{
	int prePadCount, postPadCount;

	static char consoleOut[CONSOLE_MAX_BUF_LEN];
	static char workingBuf[CONSOLE_WORKING_LEN];

	for (int i=0; i<activeNodes.nodeCount; i++)
	{
		ActiveNode *activeNode = &activeNodes.activeNode[i];

		if (activeNode)
		{
			memset (consoleOut, 0, CONSOLE_MAX_BUF_LEN);
			static char lineHdrFmt[] = "\t %2d.";
			sprintf(consoleOut, lineHdrFmt, i+1);

			/* Get the serial number */
			memset(workingBuf, 0, CONSOLE_WORKING_LEN);
			static char snFmt[] = "%015llu-%04d";
			sprintf(workingBuf, snFmt, activeNode->serialNumber.mfgId, activeNode->serialNumber.nodeId);
			prePadCount = CONSOLE_MAX_SER_NUM_LEN-strlen(workingBuf);
			consoleCenterOutputLine(consoleOut, workingBuf, &prePadCount, &postPadCount);

			/* Get the IP address */
			memset(workingBuf, 0, CONSOLE_WORKING_LEN);
			char *ipAddr = (char *)&activeNode->nodeIp;
			static char ipFmt[] = "%u.%u.%u.%u";
			sprintf(workingBuf, ipFmt,
					*(ipAddr)&0x0ff,
					*(ipAddr+1)&0x0ff,
					*(ipAddr+2)&0x0ff,
					*(ipAddr+3)&0x0ff);
			prePadCount = CONSOLE_MAX_IP_ADDR_LEN-strlen(workingBuf);
			consoleCenterOutputLine(consoleOut, workingBuf, &prePadCount, &postPadCount);

			/* Get the active disable setting */
			memset(workingBuf, 0, CONSOLE_WORKING_LEN);
			sprintf(workingBuf, "%s", activeNode->activeDisable?"Yes":"No");
			prePadCount = CONSOLE_MAX_DISABLE_LEN-strlen(workingBuf);
			consoleCenterOutputLine(consoleOut, workingBuf, &prePadCount, &postPadCount);

			/* Get the warmup time average */
			{
				unsigned int mins = activeNode->runningNodeRateOfChangeWarmupAverage/60;
				unsigned int secs = activeNode->runningNodeRateOfChangeWarmupAverage-(mins*60);
				memset(workingBuf, 0, CONSOLE_WORKING_LEN);
				sprintf(workingBuf, "%02d:%02d", mins, secs);
				prePadCount = CONSOLE_MAX_AVG_OFF_LEN-strlen(workingBuf);
				consoleCenterOutputLine(consoleOut, workingBuf, &prePadCount, &postPadCount);
			}

			/* Get the cooldown time average */
			{
				unsigned int mins = activeNode->runningNodeRateOfChangeCooldownAverage/60;
				unsigned int secs = activeNode->runningNodeRateOfChangeCooldownAverage-(mins*60);
				memset(workingBuf, 0, CONSOLE_WORKING_LEN);
				sprintf(workingBuf, "%02d:%02d", mins, secs);
				prePadCount = CONSOLE_MAX_AVG_OFF_LEN-strlen(workingBuf);
				consoleCenterOutputLine(consoleOut, workingBuf, &prePadCount, &postPadCount);
			}

			/* Get the amps average */
			memset(workingBuf, 0, CONSOLE_WORKING_LEN);
			sprintf(workingBuf, "%d", activeNode->runningNodeAmpsAverage);
			prePadCount = CONSOLE_MAX_AVG_AMP_LEN-strlen(workingBuf);
			consoleCenterOutputLine(consoleOut, workingBuf, &prePadCount, &postPadCount);

			/* Get the quiet node alert indicator */
			memset(workingBuf, 0, CONSOLE_WORKING_LEN);
			if (activeNode->nodeQuietCount > MAX_QUIET_ALERT)
				sprintf(workingBuf, "*");
			else
				sprintf(workingBuf, " ");
			prePadCount = CONSOLE_MAX_HRTBEAT_LEN-strlen(workingBuf);
			consoleCenterOutputLine(consoleOut, workingBuf, &prePadCount, &postPadCount);

			assert(strlen(consoleOut) < CONSOLE_MAX_BUF_LEN);
			printw("%s\n",consoleOut);
			refresh();
		}
	}
}

/* ========================================*/
/*
 *  consoleReportActiveNodes
 */
/* ========================================*/
void consoleReportSystemStatus(void)
{
	static char currentSysStatus[]  = "    Current System Status";
	static char numberOfOnNodes[]   = "      Number of  on nodes";
	static char avgNumOfOnNodes[]   = " Avg. number of  on nodes";
	static char numberOfOffNodes[]  = "      Number of off nodes";
	static char sysStatusLearning[] = "Learning";
	static char sysStatusActive[]   = "Active";
	static char currentAmpDraw[]    = "         Present Amp draw";
	static char averageAmpDraw[]    = "         Average Amp draw";
	char* sysStatusStr;

	if (sysStatus.status == systemStatus_Learning)
		sysStatusStr = sysStatusLearning;
	else
		sysStatusStr = sysStatusActive;

	printw("\t\t%s:\t%s\n", currentSysStatus, sysStatusStr);
	printw("\t\t%s:\t%3d\n", numberOfOnNodes, sysStatus.onNodesCount);
	printw("\t\t%s:\t%3d\n", avgNumOfOnNodes, sysStatus.runningOnNodesAverage);
	printw("\t\t%s:\t%3d\n", numberOfOffNodes, sysStatus.offNodesCount);
	printw("\t\t------------------------------------------\n");
	printw("\t\t%s:\t%3d\n", currentAmpDraw, sysStatus.currentAmps);
	printw("\t\t%s:\t%3d\n", averageAmpDraw, sysStatus.runningAmpsAverage);
	refresh();
}

/* ========================================*/
/*
 *  activeNodeCheck
 */
/* ========================================*/
void activeNodeCheck(Packet *packet)
{
	/* Look whether we've seen this node before. */
	/* If not, add it to the active node array. */
	bool nodeFound = FALSE;

	if (activeNodes.nodeCount)
	{
		if (activeNodes.nodeCount < MAX_POWERMON_NODES)
		{
			for (int i=0; i<activeNodes.nodeCount; i++)
			{
				if ((activeNodes.activeNode[i].serialNumber.mfgId == packet->node.serialNumber.mfgId) &&
					(activeNodes.activeNode[i].serialNumber.nodeId == packet->node.serialNumber.nodeId) &&
					(activeNodes.activeNode[i].nodeIp == packet->node.nodeIp))
				{
					nodeFound = TRUE;
					activeNodes.activeNode[i].nodeQuietCount = 0; /* Not quiet */
					break;
				}
			}

			if (!nodeFound)
			{
				/* Add node to the array */
				ActiveNode *activeNode = &activeNodes.activeNode[activeNodes.nodeCount];
				memset((void *)activeNode, 0, sizeof(ActiveNode));

				activeNode->serialNumber.mfgId = packet->node.serialNumber.mfgId;
				activeNode->serialNumber.nodeId = packet->node.serialNumber.nodeId;
				activeNode->nodeIp = packet->node.nodeIp;
				time(&activeNode->timestamp);

				activeNodes.nodeCount+=1;

				POWERMON_LOGGER(CALC, INFO, "Added node %015llu-%04d to activeNode array.\n",
						packet->node.serialNumber.mfgId, packet->node.serialNumber.nodeId);
			}
		}
		else
		{
			/* Array is full */
			POWERMON_LOGGER(CALC, WARN, "activeNode array is full.\n",0);
		}
	}
	else
	{
		/* First active node into array */
		/* Add node to the array */
		ActiveNode *activeNode = &activeNodes.activeNode[activeNodes.nodeCount];
		memset((void *)activeNode, 0, sizeof(ActiveNode));

		activeNode->serialNumber.mfgId = packet->node.serialNumber.mfgId;
		activeNode->serialNumber.nodeId = packet->node.serialNumber.nodeId;
		activeNode->nodeIp = packet->node.nodeIp;
		time(&activeNode->timestamp);

		activeNodes.nodeCount+=1;

		POWERMON_LOGGER(CALC, INFO, "Added node %015llu-%04d to activeNode array.\n",
				packet->node.serialNumber.mfgId, packet->node.serialNumber.nodeId);
	}
}

/* ========================================*/
/*
 *  findActiveNode
 */
/* ========================================*/
ActiveNode * findActiveNode(Packet *packet)
{
	bool nodeFound = FALSE;
	ActiveNode *activeNode;

	if (activeNodes.nodeCount)
	{
		POWERMON_LOGGER(CALC, DEBUG, "Searching for node %015llu-%04d:%08x.\n",
								packet->node.serialNumber.mfgId, packet->node.serialNumber.nodeId,
								packet->node.nodeIp);

		for (int i=0; i<activeNodes.nodeCount; i++)
		{
			POWERMON_LOGGER(CALC, TRACE, "Trying node %015llu-%04d:%08x.\n",
					activeNodes.activeNode[i].serialNumber.mfgId,
					activeNodes.activeNode[i].serialNumber.nodeId,
					activeNodes.activeNode[i].nodeIp);

			if ((activeNodes.activeNode[i].serialNumber.mfgId == packet->node.serialNumber.mfgId) &&
				(activeNodes.activeNode[i].serialNumber.nodeId == packet->node.serialNumber.nodeId) &&
				(activeNodes.activeNode[i].nodeIp == packet->node.nodeIp))
			{
				POWERMON_LOGGER(CALC, DEBUG, "Found node %015llu-%04d:%08x.\n",
						activeNodes.activeNode[i].serialNumber.mfgId,
						activeNodes.activeNode[i].serialNumber.nodeId,
						activeNodes.activeNode[i].nodeIp);

				activeNode = &activeNodes.activeNode[i];
				nodeFound = TRUE;
				break;
			}
		}

		if (!nodeFound)
			activeNode = (ActiveNode *)NULL;
	}
	else
		activeNode = (ActiveNode *)NULL;

	return activeNode;
}

/* ========================================*/
/*
 *  removeActiveNode
 */
/* ========================================*/
void removeActiveNode(unsigned int index)
{
	if (index < activeNodes.nodeCount)
	{
		POWERMON_LOGGER(CALC, INFO, "Removing node %015llu-%04d:%d.\n",
				activeNodes.activeNode[index].serialNumber.mfgId,
				activeNodes.activeNode[index].serialNumber.nodeId,
				activeNodes.activeNode[index].nodeIp);

		for (int i=index; i<activeNodes.nodeCount; i++)
			if (i == activeNodes.nodeCount-1)
				/* This is the last active node in the array. It was copied already. */
				memset((void *)&activeNodes.activeNode[i], 0, sizeof(ActiveNode));
			else
				/* Move the next into the previous */
				memcpy((void *)&activeNodes.activeNode[i], (void *)&activeNodes.activeNode[i+1], sizeof(ActiveNode));

		activeNodes.nodeCount-=1;
	}
	else
		POWERMON_LOGGER(CALC, WARN, "Active node index (%d) too large.\n",index);
}

/* ========================================*/
/*
 *  addToPollCycleData
 */
/* ========================================*/
void addToPollCycleData(Packet *packet)
{
	activeNodeCheck(packet);

	pthread_mutex_lock(&pwrmonActiveDataArrayMutex);
	if (pollCycleData[activeDataArray].index < MAX_POLL_CYCLE_PKTS)
	{
		unsigned int index = pollCycleData[activeDataArray].index;
		memcpy(&pollCycleData[activeDataArray].packet[index], packet, sizeof(Packet));
		pollCycleData[activeDataArray].index+=1;
	}
	pthread_mutex_unlock(&pwrmonActiveDataArrayMutex);
}

/* ========================================*/
/*
 *  initPollCycleDataArray
 */
/* ========================================*/
void initPollCycleDataArray(unsigned int array)
{
	if (array < MAX_POLL_CYCLE_DATA_ARRAYS)
	{
		memset(pollCycleData[array].packet, 0, sizeof(PollCycleData));
		pollCycleData[array].index = 0;
	}
}
/* ========================================*/
/*
 *  updateCoolingNodeToIdle
 *
 *  Previously on node has turned off.
 *  Cool node will begin warming.
 *
 */
/* ========================================*/
void updateCoolingNodeToIdle(ActiveNode *activeNode)
{
	time_t currTime;
	double elapsedTime;

	time(&currTime);
	elapsedTime = getPwrmonElapsedTime(currTime, activeNode->timestamp, NULL);
	activeNode->timestamp = currTime;

	activeNode->nodeOn = FALSE;

	/* Update this node's info */
	activeNode->runningNodeRateOfChangeWarmupCountTotal[activeNode->runningNodeOffIndex] = elapsedTime;
	activeNode->runningNodeOffCountTotal[activeNode->runningNodeOffIndex] = activeNode->nodeOffCount;

	if ((activeNode->runningNodeOffIndex+=1) == MAX_COUNT_SAMPLES)
		activeNode->runningNodeOffIndex = 0;

	activeNode->nodeOffCount = 0; /* Reset the count to zero */

	/* Re-calc running averages */
	activeNode->runningNodeOffAverage = 0;
	activeNode->runningNodeRateOfChangeWarmupAverage = 0;
	for (int i=0; i<MAX_COUNT_SAMPLES; i++)
	{
		activeNode->runningNodeOffAverage += activeNode->runningNodeOffCountTotal[i];
		activeNode->runningNodeRateOfChangeWarmupAverage += activeNode->runningNodeRateOfChangeWarmupCountTotal[i];
	}
	activeNode->runningNodeOffAverage /= MAX_COUNT_SAMPLES;
	activeNode->runningNodeRateOfChangeWarmupAverage /= MAX_COUNT_SAMPLES;
}

/* ========================================*/
/*
 *  updateIdleNodeToCooling
 *
 *  Previously off node has turned on.
 *  Warm node will begin cooling.
 */
/* ========================================*/
void updateIdleNodeToCooling(ActiveNode *activeNode)
{
	time_t currTime;
	double elapsedTime;

	time(&currTime);
	elapsedTime = getPwrmonElapsedTime(currTime, activeNode->timestamp, NULL);
	activeNode->timestamp = currTime;

	activeNode->nodeOn = TRUE;

	/* Update this node's info */
	activeNode->runningNodeOnCountTotal[activeNode->runningNodeOnIndex] = activeNode->nodeOnCount;
	activeNode->runningNodeRateOfChangeCooldownCountTotal[activeNode->runningNodeOnIndex] = elapsedTime;
	activeNode->runningNodeAmpsCountTotal[activeNode->runningNodeOnIndex] = activeNode->nodeOnAmpsCount;

	if ((activeNode->runningNodeOnIndex+=1) == MAX_COUNT_SAMPLES)
		activeNode->runningNodeOnIndex = 0;

	activeNode->nodeOnCount = 0; /* Reset the count to zero */
	activeNode->nodeOnAmpsCount = 0; /* Reset the count to zero */

	/* Re-calc running average */
	activeNode->runningNodeOnAverage = 0;
	activeNode->runningNodeRateOfChangeCooldownAverage = 0;
	activeNode->runningNodeAmpsAverage = 0;

	for (int i=0; i<MAX_COUNT_SAMPLES; i++)
	{
		activeNode->runningNodeOnAverage += activeNode->runningNodeOnCountTotal[i];
		activeNode->runningNodeRateOfChangeCooldownAverage += activeNode->runningNodeRateOfChangeCooldownCountTotal[i];
		activeNode->runningNodeAmpsAverage += activeNode->runningNodeAmpsCountTotal[i];
	}
	activeNode->runningNodeOnAverage /= MAX_COUNT_SAMPLES;
	activeNode->runningNodeRateOfChangeCooldownAverage /= MAX_COUNT_SAMPLES;
	activeNode->runningNodeAmpsAverage /= MAX_COUNT_SAMPLES;
}

/* ========================================*/
/*
 *  updateActiveNodeData
 *
 *  Visited every poll interval (60 seconds)
 */
/* ========================================*/
void updateActiveNodeData(unsigned int array)
{
	POWERMON_LOGGER(CALC, INFO, "Updating active nodes from array %d.\n",array);
	POWERMON_LOGGER(CALC, INFO, "%d entries in pollCycleData[%d]\n", pollCycleData[array].index, array);

	for (int i=0; i<pollCycleData[array].index; i++)
	{
		Packet *packet = &pollCycleData[array].packet[i];
		ActiveNode *activeNode = findActiveNode(packet);

		if (activeNode)
		{
			POWERMON_LOGGER(CALC, DEBUG, "Updating active node: %015llu-%04d.\n",
					activeNode->serialNumber.mfgId, activeNode->serialNumber.nodeId);

			activeNode->temp = packet->data.temp;
			activeNode->amps = packet->data.amps;
			activeNode->defrosting = (packet->node.operation == Defrost);

			if (packet->data.amps == 0) /* ================ Node is idle ================ */
			{
				if (activeNode->nodeOn == TRUE) /* Did it just stop running? */
				{
					POWERMON_LOGGER(CALC, INFO, "Active node %015llu-%04d just went idle. Update.\n",
							activeNode->serialNumber.mfgId, activeNode->serialNumber.nodeId);

					/* If so indicate it is no longer cooling and update */
					updateCoolingNodeToIdle(activeNode);
				}
				else
					POWERMON_LOGGER(CALC, TRACE, "Active node %015llu-%04d idle.\n",
							activeNode->serialNumber.mfgId, activeNode->serialNumber.nodeId);

				activeNode->nodeOffCount += 1;
			}
			else /* ================ Node is cooling ================ */
			{
				if (activeNode->nodeOn == FALSE) /* Did it just start running? */
				{
					POWERMON_LOGGER(CALC, INFO, "Active node %015llu-%04d just started cooling. Update.\n",
							activeNode->serialNumber.mfgId, activeNode->serialNumber.nodeId);

					/* If so indicate it is now cooling and update */
					updateIdleNodeToCooling(activeNode);
				}
				else
					POWERMON_LOGGER(CALC, TRACE, "Active node %015llu-%04d cooling.\n",
							activeNode->serialNumber.mfgId, activeNode->serialNumber.nodeId);

				activeNode->nodeOnCount += 1;
				activeNode->nodeOnAmpsCount += activeNode->amps;
				sysStatus.runningAmps += activeNode->amps;
			}
		}
		else
			POWERMON_LOGGER(CALC, WARN, "Couldn't find active node: %015llu-%04d.\n",
					packet->node.serialNumber.mfgId, packet->node.serialNumber.nodeId);
	}

	/* Cleanup */
	initPollCycleDataArray(array);
}

/* ========================================*/
/*
 *  doActiveNodeSort
 *  insertIntoOnList
 *  insertIntoOffList
 *
 *  Sort the lists of active nodes:
 *  hottest to coldest
 *  coldest to hottest
 */
/* ========================================*/
void insertIntoOnList(ActiveNode *newNode)
{
	ActiveNode *node = newNode;
	bool done = FALSE;
	int index = 0;

	do {

		if (sysStatus.onNodes[index] == (ActiveNode *)NULL)
		{
			sysStatus.onNodes[index] = (void *)node;
			sysStatus.onNodesCount+=1;
			done = TRUE;
		}
		else
		{
			ActiveNode *listNode = (ActiveNode *)sysStatus.onNodes[index];

			if (listNode->temp > node->temp)
			{
				ActiveNode *swapNode = listNode;
				sysStatus.onNodes[index] = (void *)node;
				node = swapNode;
			}

			if ((node == (ActiveNode *)NULL) ||
				((index += 1) == activeNodes.nodeCount))
			{
				sysStatus.onNodesCount+=1;
				done = TRUE;
			}
		}

	} while (!done);

}

void insertIntoOffList(ActiveNode *newNode)
{
	ActiveNode *node = newNode;
	bool done = FALSE;
	int index = 0;

	do {

		if (sysStatus.offNodes[index] == (ActiveNode *)NULL)
		{
			sysStatus.offNodes[index] = (void *)node;
			sysStatus.offNodesCount+=1;
			done = TRUE;
		}
		else
		{
			ActiveNode *listNode = (ActiveNode *)sysStatus.offNodes[index];

			if (listNode->temp < node->temp)
			{
				ActiveNode *swapNode = listNode;
				sysStatus.offNodes[index] = (void *)node;
				node = swapNode;
			}

			if ((node == (ActiveNode *)NULL) ||
				((index += 1) == activeNodes.nodeCount))
			{
				sysStatus.offNodesCount+=1;
				done = TRUE;
			}
		}

	} while (!done);
}

void doActiveNodeSort()
{
	memset(sysStatus.offNodes, 0, sizeof(sysStatus.offNodes));
	memset(sysStatus.onNodes, 0, sizeof(sysStatus.onNodes));
	sysStatus.onNodesCount = 0;
	sysStatus.offNodesCount = 0;

	for (int i=0; i<activeNodes.nodeCount; i++)
	{
		if ((activeNodes.activeNode[i].nodeOn) && (!activeNodes.activeNode[i].defrosting))
			insertIntoOnList(&activeNodes.activeNode[i]);
		else
			insertIntoOffList(&activeNodes.activeNode[i]);
	}

	/* Log the sorted lists */
	POWERMON_LOGGER(CALC, TRACE, "\tOff List (%d 0f %d)\n", sysStatus.offNodesCount, activeNodes.nodeCount);
	POWERMON_LOGGER(CALC, TRACE, "\t---------------------\n",0);
	for (int i=0; i<sysStatus.offNodesCount; i++)
		if (sysStatus.offNodes[i])
			POWERMON_LOGGER(CALC, TRACE, "\tnodeId: %04d, \ttemp: %.2f\n",
					((ActiveNode *)sysStatus.offNodes[i])->serialNumber.nodeId, (float)(((ActiveNode *)sysStatus.offNodes[i])->temp)/100);

	POWERMON_LOGGER(CALC, TRACE, "\tOn List (%d 0f %d)\n", sysStatus.onNodesCount, activeNodes.nodeCount);
	POWERMON_LOGGER(CALC, TRACE, "\t---------------------\n",0);
	for (int i=0; i<sysStatus.onNodesCount; i++)
		if (sysStatus.onNodes[i])
			POWERMON_LOGGER(CALC, TRACE, "\tnodeId: %04d, \ttemp: %.2f\n",
					((ActiveNode *)sysStatus.onNodes[i])->serialNumber.nodeId, (float)(((ActiveNode *)sysStatus.onNodes[i])->temp)/100);
}

/* ========================================*/
/*
 *  doSystemPowerCalc
 *
 *  Visited every poll interval (60 seconds)
 */
/* ========================================*/
void doSystemPowerCalc(void)
{
	/* On node stats */
	sysStatus.runningOnNodesCountTotal[sysStatus.runningOnNodesIndex] = sysStatus.onNodesCount;
	if ((sysStatus.runningOnNodesIndex+=1) == MAX_COUNT_SAMPLES)
		sysStatus.runningOnNodesIndex = 0;

	/* Amp draw stats */
	sysStatus.currentAmps = sysStatus.runningAmps;
	sysStatus.runningAmpsCountTotal[sysStatus.runningAmpsIndex] = sysStatus.runningAmps;
	sysStatus.runningAmps = 0;
	if ((sysStatus.runningAmpsIndex+=1) == MAX_COUNT_SAMPLES)
		sysStatus.runningAmpsIndex = 0;

	/* Running averages */
	for (int i=0; i<MAX_COUNT_SAMPLES; i++)
	{
		sysStatus.runningOnNodesAverage += sysStatus.runningOnNodesCountTotal[i];
		sysStatus.runningAmpsAverage += sysStatus.runningAmpsCountTotal[i];
	}
	sysStatus.runningOnNodesAverage /= MAX_COUNT_SAMPLES;
	sysStatus.runningAmpsAverage /= MAX_COUNT_SAMPLES;
}

/* ========================================*/
/*
 *  doPowerCalcAnalysis
 *
 *  Visited every poll interval (60 seconds)
 */
/* ========================================*/
bool doPowerCalcAnalysis(void)
{
	POWERMON_LOGGER(CALC, INFO, "Performing power consumption analysis from node data.\n",0);

	if (sysStatus.status == systemStatus_Learning)
	{
		PwrmonTime elapsedTime;

		getPwrmonElapsedSystemTime(&elapsedTime);
		if (elapsedTime.days >= LEARNING_CYCLE_DAYS)
			sysStatus.status = systemStatus_Managing;
	}

	if (activeNodes.nodeCount)
	{
		doActiveNodeSort();
		doSystemPowerCalc();
	}

	return (sysStatus.status == systemStatus_Managing);
}

/* ========================================*/
/*
 *  doPowerCalc
 *
 *  Visited every poll interval (60 seconds)
 */
/* ========================================*/
void doPowerCalc(void)
{
	POWERMON_LOGGER(CALC, DEBUG, "Performing power calculations from node data.\n",0);

	if (activeNodes.nodeCount)
	{
		doPowerCalcAnalysis();

		/* Who's on, who's off, who will need to go on, and who can afford to go off? */
		/* How close does off count need to be to 'off' avg to be a must turn on? */
		/* How close does on count need to be to 'on' avg to be eligible for idling? */
		/* When an on node gets turned off, how do we know when it must be turned back on? */
		/* When a node is told to turn off but it doesn't assume it's defrosting. */
		for (int i=0; i<activeNodes.nodeCount; i++)
		{
			/* Turn the cold/on node off */
			/* Turn the hot/off node on */
		}
	}
}

/* ========================================*/
/*
 *  activeNodeHealthCheck
 *
 *  Visited every poll interval (60 seconds)
 *
 *  Like a heart beat in reverse.
 *  Mark all nodes as quiet during each poll cycle.
 *  As the nodes' packets get processed, set
 *  the quiet node indication to zero (not quiet).
 *  If we haven't heard from a node in an hour
 *  (quiet == 60) send an alert indicating so.
 *  If we haven't heard from the node within 3 days
 *  (60 * 24 * 3) remove it from the active node array.
 */
/* ========================================*/
void activeNodeHealthCheck(void)
{
	for (int i=0; i<activeNodes.nodeCount; i++)
	{
		activeNodes.activeNode[i].nodeQuietCount+=1;

		POWERMON_LOGGER(CALC, DEBUG, "Node %015llu-%04d: Quiet count: %d\n",
				activeNodes.activeNode[i].serialNumber.mfgId,
				activeNodes.activeNode[i].serialNumber.nodeId,
				activeNodes.activeNode[i].nodeQuietCount);

		if (activeNodes.activeNode[i].nodeQuietCount > MAX_QUIET_REMOVE)
			removeActiveNode(i);
	}
}
/* ========================================*/
/*
 *  powermonCalc
 *
 *  Visited every poll interval (60 seconds)
 */
/* ========================================*/
void powermonCalc(unsigned int array)
{
	static bool activeCalc = FALSE;

	POWERMON_LOGGER(CALC, TRACE, "PowerMon thread.\n",0);

	activeNodeHealthCheck();		/* Look to see if any nodes have dropped off */
	updateActiveNodeData(array);	/* Update the array of currently known nodes */

	if (activeCalc)
		doPowerCalc();				/* Turn nodes off/on that need to be turned off/on */
	else
		activeCalc = doPowerCalcAnalysis();
}

/* ========================================*/
/*
 *  powermon_calc_thread_proc
 */
/* ========================================*/
void* powermon_calc_thread_proc(void* arg)
{
	unsigned int *threadActive = (unsigned int *)arg;

	POWERMON_LOGGER(CALC, THREAD, "powermon_calc_thread is alive.\n",0);

	/* Perform some initialization housekeeping */
	activeNodes.nodeCount = 0;
	for (int i=0; i<MAX_POWERMON_NODES; i++)
		memset((void *)&activeNodes.activeNode[i], 0, sizeof(ActiveNode));

	memset((void *)&sysStatus, 0, sizeof(PwrCalcSystemStatus));

	sysStatus.status = systemStatus_Learning;

	initPollCycleDataArray(0);
	initPollCycleDataArray(1);
	activeDataArray = 0;

	do {
		static unsigned int processArray;

		sleep(CALC_INTERVAL); /* Once a minute */

		pthread_mutex_lock(&pwrmonActiveDataArrayMutex);

		processArray = activeDataArray;

		if (activeDataArray)
			activeDataArray = 0;
		else
			activeDataArray = 1;

		pthread_mutex_unlock(&pwrmonActiveDataArrayMutex);

		powermonCalc(processArray);

	} while (*threadActive);

	POWERMON_LOGGER(CALC, THREAD, "Exiting powermon_calc thread.\n",0);

	pthread_exit((void *)NULL);

	return (void *)NULL;
}

/* ========================================*/
/*
 *  powermon_calc_thread_create
 */
/* ========================================*/
pthread_t powermon_calc_thread_create(unsigned int *calc_thread_active)
{
	static pthread_t powermon_calc_tid = (pthread_t)NULL;

	int err = pthread_create(
			&powermon_calc_tid,
			NULL,
			&powermon_calc_thread_proc,
			(void *)calc_thread_active);

	if (err != 0)
	{
		POWERMON_LOGGER(CALC, FATAL, "Unable to create powermon_thread :[%s]\n", strerror(err));
		powermon_calc_tid = 0;
	}

	return powermon_calc_tid;
}
