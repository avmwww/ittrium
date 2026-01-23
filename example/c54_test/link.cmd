/*****************************************************************************/
/*                                                                           */
/* LNK.CMD - V2.00  COMMAND FILE FOR LINKING C PROGRAMS                      */
/*                                                                           */
/*      Usage:  lnk500 <obj files...>    -o <out file> -m <map file> lnk.cmd */
/*              cl500  <src files...> -z -o <out file> -m <map file> lnk.cmd */
/*                                                                           */
/*      Description: This file is a sample command file that can be used     */
/*                   for linking programs built with the C54x C Compiler.    */
/*                   This file has been designed to work for                 */
/*                   548 C54x device.                                        */
/*                   Use it as a guideline; you may want to make alterations */
/*                   appropriate for the memory layout of the target         */
/*                   system and/or your application.                         */
/*                                                                           */
/*      Notes: (1)   You must specify the directory in which rts.lib is      */
/*                   located.  Either add a "-i<directory>" line to this     */
/*                   file, or use the system environment variable C_DIR to   */
/*                   specify a search path for the libraries.                */
/*                                                                           */
/*             (2)   If the run-time library you are using is not            */
/*                   named rts.lib, be sure to use the correct name here.    */
/*****************************************************************************/

/* choose initialization strategy (-c or -cr): */
/*-cr     downloader initializes global variables (not cold boot)   */

/*-c  copy constants in .cinit to global variables at boot time */
/* -x */
-c
-l rts.lib
/*-l rts.lib  must go AFTER -c or -cr */

/* specify sizes of stack & heap: */
-stack 0x800 /* size of .stack section */
-heap  0x800 /* size of .sysmem section */
-m out.map

MEMORY {
   PAGE 0: /* program memory */

      /* IF       THIS IS                               */
      /* ===      =======                               */
      /* OVLY=1   some of internal RAM and/or external  */
      /*          RAM depending on which C54x this is.  */
      /* OVLY=0   external RAM                          */
	PROG_RAM (RWX) : origin = 0x0100, length = 0x2F00
	PROG_EXT (RWX) : origin = 0x8000, length = 0x4000
   
      /* boot interrupt vector table location */
        VECTORS (RWX): origin = 0x0080, length = 0x80


   PAGE 1: /* data memory, addresses 0-7Fh are reserved */

      /* some (or all) of internal DARAM */
	DATA_RAM (RW): origin = 0x3000, length = 0x2000
	DATA_EXT (RW): origin = 0x8000, length = 0x7FFF


   PAGE 2: /* I/O memory */

      /* no devices declared */

} /* MEMORY */


SECTIONS {
   .text    >> PROG_RAM | PROG_EXT PAGE 0   /* code                     */
   .switch  > PROG_RAM PAGE 0               /* switch table info        */

/* .cinit must be allocated to PAGE 0 when using -c (vs -cr)            */
/* .cinit   > PROG_RAM PAGE 0 *//* commented because -cr used above     */

   .vectors > VECTORS PAGE 0               /* interrupt vectors         */

   .cio     >  DATA_RAM PAGE 1             /* C I/O                     */  
   .data    >> DATA_RAM | DATA_EXT PAGE 1  /* initialized data          */
   .bss     >> DATA_RAM | DATA_EXT PAGE 1  /* global & static variables */
   .const   >  DATA_RAM PAGE 1             /* constant data             */
   .sysmem  >> DATA_RAM | DATA_EXT PAGE 1  /* heap                      */
   .stack   >> DATA_RAM | DATA_EXT PAGE 1  /* stack                     */
} /* SECTIONS */
