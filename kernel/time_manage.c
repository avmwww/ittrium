/* ittrium kernel — time_manage.c
 * Copyright (c) 2004-2026 Andrey Mitrofanov
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "ittrium.h"

extern SYSTIM _system_ticks;

ER set_tim ( SYSTIM *p_systim )
{
  BEGIN_CRITICAL_SECTION;
  _system_ticks.st_low = p_systim->st_low;
  _system_ticks.st_high = p_systim->st_high;
  END_CRITICAL_SECTION;
  return E_OK;
}

ER get_tim ( SYSTIM *p_systim )
{
  BEGIN_CRITICAL_SECTION;
  p_systim->st_low = _system_ticks.st_low;
  p_systim->st_high = _system_ticks.st_high;
  END_CRITICAL_SECTION;
  return E_OK;
}

