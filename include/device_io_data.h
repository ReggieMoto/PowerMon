/* ============================================================== */
/*
 * device_io_data.h
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
 *  The contents of this file must contain the same structure and
 *  configuration values as the similar file in the unit test.
 */
/* ============================================================== */

#ifndef __DEVICE_IO_DATA_H__
#define __DEVICE_IO_DATA_H__

#include <stdbool.h>
#include <time.h>
#include "common.h"
#include "powermon_pkt.h"

#define MAX_POLL_CYCLE_DATA_ARRAYS 2
#define POLL_CYCLES_PER_MINUTE 1
#define MAX_PACKETS_PER_MINUTE (10 * POLL_CYCLES_PER_MINUTE) /* 10 per node per minute */
#define MAX_POLL_CYCLE_PKTS (MAX_POWERMON_NODES * MAX_PACKETS_PER_MINUTE)

#define MAX_COUNT_SAMPLES 7

/*
 * Represents a node being managed by PowerMon
 */
typedef struct _activeNode {
	SerialNumber_t serialNumber;
	unsigned int nodeIp;

	/* ======================================= */
	/* Current State */
	time_t timestamp; /* Time of discovery or last transition off/on/off */

	bool nodeOn;		/* Is the node currently on/true or off/false */
	bool defrosting;	/* Is the node currently defrosting/true or not/false */
	unsigned int temp;	/* The node's current temp */
	unsigned int amps;	/* The number of amps the node is pulling */
	bool activeDisable;	/* Did we turn this node off */

	unsigned int nodeQuietCount;	/* Indication of quiet node */

	/* ======================================= */
	/* Historical data */
	/* ======================================= */
	/* On/Cooling Node */

	unsigned int nodeOnCount;		/* Current count of 'on' pkts for this cycle */
	unsigned int runningNodeOnIndex; /* Runs from 0 to MAX_ON_COUNT_TOTALS */
	unsigned int runningNodeOnCountTotal[MAX_COUNT_SAMPLES];	/* For calculating the running avg */
	unsigned int runningNodeOnAverage;	/* The running average */

	double runningNodeRateOfChangeCooldownCountTotal[MAX_COUNT_SAMPLES];	/* For calculating the running avg */
	double runningNodeRateOfChangeCooldownAverage;	/* The running average */

	unsigned int nodeOnAmpsCount;	/* Current count of amp draw for this cycle */
	unsigned int runningNodeAmpsCountTotal[MAX_COUNT_SAMPLES];	/* For calculating the running avg */
	unsigned int runningNodeAmpsAverage;	/* The running average */

	/* ======================================= */
	/* Off/Warming Node */

	unsigned int nodeOffCount;		/* Current count of 'off' pkts for this cycle */
	unsigned int runningNodeOffIndex; /* Runs from 0 to MAX_ON_COUNT_TOTALS */

	unsigned int runningNodeOffCountTotal[MAX_COUNT_SAMPLES];	/* For calculating the running avg */
	unsigned int runningNodeOffAverage;	/* The running average */

	double runningNodeRateOfChangeWarmupCountTotal[MAX_COUNT_SAMPLES];	/* For calculating the running avg */
	double runningNodeRateOfChangeWarmupAverage;	/* The running average */

} ActiveNode;

typedef struct _activeNodeArray {
	unsigned int nodeCount;
	ActiveNode activeNode[MAX_POWERMON_NODES];
} ActiveNodeArray;

typedef struct _pollCycleData {
	unsigned int index;
	Packet packet[MAX_POLL_CYCLE_PKTS];
} PollCycleData;

#endif /* __DEVICE_IO_DATA_H__ */
