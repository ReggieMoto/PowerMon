/* ============================================================== */
/*
 * powermon_curses.h
 *
 * Copyright (c) 2020 David Hammond
 * All Rights Reserved.
 *
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


#include "ncurses.h"

#ifndef __POWERMON_CURSES_H__
#define __POWERMON_CURSES_H__

int initialize_curses(void);
void close_curses(void);

#endif /* __POWERMON_CURSES_H__ */
