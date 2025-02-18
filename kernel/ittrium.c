/******************************************************************************
**	Filename:		ittrium.c
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
/*******************************************************************************
 Kernel master file
 *******************************************************************************/

#include "main.c"
#include "eventflags.c"
#include "mailbox.c"
#include "semaphore.c"
#include "task.c"
#include "time_manage.c"
#include "timer.c"
#include "tsk_dep_syn.c"
#include "tsk_manage.c"
#include "wait.c"
#include "cpu_support.c"
