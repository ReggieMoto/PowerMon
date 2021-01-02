/* ============================================================== */
/*
 * powermon_calc.h
 *
 * Copyright (c) 2018 David Hammond
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

#ifndef __POWERMON_CALC_H__
#define __POWERMON_CALC_H__

#include "common.h"

#define MAX_QUEUE_DEPTH MAX_POWERMON_NODES
#define MAX_COUNT_SAMPLES 7

typedef enum e_systemStatus {
	systemStatus_Learning,
	systemStatus_Managing
} SystemStatus;

typedef struct _pwrCalcSystemStatus {

	SystemStatus status;

	/* Amps */
	unsigned int currentAmps;
	unsigned int targetAmps;

	unsigned int runningAmps;
	unsigned int runningAmpsIndex; /* Runs from 0 to MAX_ON_COUNT_TOTALS */
	unsigned int runningAmpsCountTotal[MAX_COUNT_SAMPLES];	/* For calculating the running avg */
	unsigned int runningAmpsAverage;	/* The running average */

	/* Nodes */
	unsigned int maxOnNodes;
	unsigned int targetOnNodes;
	unsigned int runningOnNodesIndex; /* Runs from 0 to MAX_ON_COUNT_TOTALS */
	unsigned int runningOnNodesCountTotal[MAX_COUNT_SAMPLES];	/* For calculating the running avg */
	unsigned int runningOnNodesAverage;	/* The running average */

	/* Sorted lists of on/off nodes */
	unsigned int offNodesCount;
	void *offNodes[MAX_QUEUE_DEPTH];	/* Sorted hottest to coldest */

	unsigned int onNodesCount;
	void *onNodes[MAX_QUEUE_DEPTH];		/* Sorted coldest to hottest */

} PwrCalcSystemStatus;

#endif /* __POWERMON_CALC_H__ */
