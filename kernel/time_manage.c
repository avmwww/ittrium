/******************************************************************************
**	Filename:		time_manage.c
**	Purpose:		
**	Author:			Andrey Mitrofanov
**	Environment:		ittrium (mITRON) Real Time Kernel
**	History:
**
**	Version:		1.0
**	Notes:			
**
**	(c) 2004 Copyright Andrey Mitrofanov.
**	Copying or other reproduction of
**	this program except for archival purposes is prohibited
**	without the prior written consent of author.
*******************************************************************************/
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

