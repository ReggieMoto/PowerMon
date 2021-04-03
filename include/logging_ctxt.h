/* ============================================================== */
/*
 * logging_ctxt.h
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

#ifndef __INCLUDE_LOGGING_CTXT_H__
#define __INCLUDE_LOGGING_CTXT_H__

#include "pwr_mon_msg.h"

void logging_context(void);
void cfg_logging_input(pwrmon_msg_t *msg);


#endif /* __INCLUDE_LOGGING_CTXT_H__ */
