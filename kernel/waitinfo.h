/* ittrium kernel — waitinfo.h
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef _WAITINFO_H_
#define _WAITINFO_H_

#include "ittrium.h"

struct task_control_block;

typedef struct {
  FLGPTN waiptn;
  MODE wfmode;
  FLGPTN *p_flgptn;
} WAITINFO_FLG;

typedef struct {
  T_MSG **ppk_msg;
} WAITINFO_MBX;

typedef union {
  WAITINFO_FLG flg;
  WAITINFO_MBX mbx;
} WAITINFO;

typedef struct {
  UINT tskwait;
  void (*chg_pri_hook)(struct task_control_block *, INT);
  void (*rel_wai_hook)(struct task_control_block *);
} WAITSPEC;

#endif /* _WAITINFO_H_ */
