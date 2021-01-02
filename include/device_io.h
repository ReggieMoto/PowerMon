/* ============================================================== */
/*
 * device_io.h
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
#ifndef __DEVICE_IO_H__
#define __DEVICE_IO_H__

#include <semaphore.h>
#include <pthread.h>

pthread_t get_device_io_tid(void);
void device_io_thread_create(sem_t *sem);

#endif /* __DEVICE_IO_H__ */
