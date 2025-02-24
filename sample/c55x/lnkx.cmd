/******************************************************************************/
/* Special lnk.cmd used for testing Large Memory Model C code                 */
/******************************************************************************/

/******************************************************************************/
/* LNK.CMD - COMMAND FILE FOR LINKING C PROGRAMS                              */
/*                                                                            */
/*      Usage:  lnk55 <obj files...>    -o <out file> -m <map file> lnk.cmd   */
/*              cl55  <src files...> -z -o <out file> -m <map file> lnk.cmd   */
/*                                                                            */
/*      Description: This file is a sample command file that can be used      */
/*                   for linking programs built with the C Compiler.          */
/*                   Use it as a guideline; you  may want to change the       */
/*                   allocation scheme according to the size of your program  */
/*                   and the memory layout of your target system.             */
/*                                                                            */
/*      Notes: (1)   You must specify the directory in which rts55.lib is     */
/*                   located.  Either add a "-i<directory>" line to this      */
/*                   file, or use the system environment variable C_DIR to    */
/*                   specify a search path for the libraries.                 */
/*                                                                            */
/*             (2)   If the run-time library you are using is not             */
/*                   named rts55.lib, be sure to use the correct name here.   */
/*                                                                            */
/******************************************************************************/

-c                    /* Use C linking conventions: auto-init vars at runtime */
-u _Reset             /* Force load of reset interrupt handler                */

/* SPECIFY THE SYSTEM MEMORY MAP */

MEMORY
{
 PAGE 0:  /* ---- Unified Program/Data Address Space ---- */

  MMR    (RWIX): origin = 0x000000, length = 0x0000c0  /* MMRs */
  DARAM0 (RWIX): origin = 0x0000c0, length = 0x00ff40  /*  64Kb - MMRs */
  SARAM0 (RWIX): origin = 0x010000, length = 0x010000  /*  64Kb */
  SARAM1 (RWIX): origin = 0x020000, length = 0x020000  /* 128Kb */
  SARAM2 (RWIX): origin = 0x040000, length = 0x010000  /*  64Kb */

  PDROM   (RIX): origin = 0xff8000, length = 0x008000  /*  32Kb */

 PAGE 2:  /* -------- 64K-word I/O Address Space -------- */

  IOPORT (RWI) : origin = 0x000000, length = 0x020000
}
 
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY */

SECTIONS
{
   .text     >> SARAM1|SARAM2|SARAM0  /* Code                        */

   /* Both stacks must be on same physical memory page               */
   .stack    >  DARAM0                /* Primary system stack        */
   .sysstack >  DARAM0                /* Secondary system stack      */

   .bss      >> DARAM0|SARAM0|SARAM1  /* Global & static vars        */
   .data     >> DARAM0|SARAM0|SARAM1  /* Initialized vars            */
   .const    >> DARAM0|SARAM0|SARAM1  /* Constant data               */
   .sysmem   > DARAM0|SARAM0|SARAM1  /* Dynamic memory (malloc)     */
   .switch   >  SARAM2                /* Switch statement tables     */
   .cinit    >  SARAM2                /* Auto-initialization tables  */
   .pinit    >  SARAM2                /* Initialization fn tables    */
   .cio      >  SARAM2                /* C I/O buffers               */

   vectors: 0x000100                       /* Interrupt vectors           */

   .ioport   >  IOPORT PAGE 2         /* Global & static ioport vars */
}
