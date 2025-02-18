/******************************************************************************
**	Filename:		winfo.h
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
#ifndef _WINFO_H_
#define _WINFO_H_

#include "ittrium.h"

struct task_control_block;

typedef struct {
  FLGPTN waiptn;
  MODE wfmode;
  FLGPTN *p_flgptn;
} WINFO_FLG;

typedef struct {
  T_MSG **ppk_msg;
}WINFO_MBX;

typedef union {
  WINFO_FLG flg;
  WINFO_MBX mbx;
} WINFO;

//
// Definition of wait specification structure
//
typedef struct {
 // Wait factor 
 UINT	tskwait;					
 //Process at task priority   change 
 void	(*chg_pri_hook)(struct task_control_block*, INT);
 //Process at task wait release 
 void	(*rel_wai_hook)(struct task_control_block*);			
} WSPEC;

#endif // _WINFO_H_
