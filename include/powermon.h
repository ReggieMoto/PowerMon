/* ============================================================== */
/*
 * powermon.h
 *
 * Copyright (c) 2020 David Hammond
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
#ifndef __POWERMON_H__
#define __POWERMON_H__

#include <semaphore.h>
#include <pthread.h>

pthread_t get_powermon_tid(void);
void* powermon_thread_create(sem_t *sem);

#endif /* __POWERMON_H__ */
