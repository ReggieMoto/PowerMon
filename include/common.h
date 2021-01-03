/* ============================================================== */
/*
 * common.h
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

#ifndef __COMMON_H__
#define __COMMON_H__

#define TRUE (1==1)
#define FALSE (1==0)

#define SUCCESS 0
#define FAILURE -1

typedef unsigned int bool;

#define MAX_KBD_INPUT_STR_LEN 32

#define SRC_ADDR_LEN 14;

#define MAX_POWERMON_NODES 48

#define POWERMON_LOG_DIR "/home/PowerMon/log/"
#define POWERMON_COREDUMP_DIR "/home/PowerMon/core/"

#endif /* __COMMON_H__ */
