#ifndef _TMS320C55_H_
#define _TMS320C55_H_

/*******************************************************************************
 External bus selection register
 *******************************************************************************/
#define EBSR         (*((volatile ioport int*)0x6C00))

#define IVPD         (*(volatile int*)0x0049)
#define IVPH         (*(volatile int*)0x004A)

/*******************************************************************************
 Clock Mode Register
 *******************************************************************************/
#define CLKMD         (*(volatile ioport int*)0x1C00)
// Mask
#define CLKMD_IAI               0x4000
#define CLKMD_IOB               0x2000
#define CLKMD_TEST              0x1000
#define CLKMD_PLL_MULT          0x0F80
#define CLKMD_PLL_DIV           0x0060
#define CLKMD_PLL_ENABLE        0x0010
#define CLKMD_BYPASS DIV        0x000C
#define CLKMD_BREAKLN           0x0002
#define CLKMD_LOCK              0x0001
// Bits
#define _CLKMD_IAI_             14
#define _CLKMD_IOB_             13
#define _CLKMD_TEST_            12
#define _CLKMD_PLL_MULT_         7
#define _CLKMD_PLL_DIV_          5
#define _CLKMD_PLL_ENABLE_       4
#define _CLKMD_BYPASS_DIV_       2
#define _CLKMD_BREAKLN_          1
#define _CLKMD_LOCK_             0
/*******************************************************************************
 EMIF module
 *******************************************************************************/
#define EGCR               (*(volatile ioport int*)0x0800) // EMIF Global Control Register xxxx xxxx 0010 xx00
#define EMI_RST            (*(volatile ioport int*)0x0801) // EMIF Global Reset Register xxxx xxxx xxxx xxxx
#define EMI_BE             (*(volatile ioport int*)0x0802) // EMIF Bus Error Status Register xx00 0000 0000 0000
#define CE0_1              (*(volatile ioport int*)0x0803) // EMIF CE0 Space Control Register 1 x010 1111 1111 1111
#define CE0_2              (*(volatile ioport int*)0x0804) // EMIF CE0 Space Control Register 2 0100 1111 1111 1111
#define CE0_3              (*(volatile ioport int*)0x0805) // EMIF CE0 Space Control Register 3 xxxx xxxx 0000 0000
#define CE1_1              (*(volatile ioport int*)0x0806) // EMIF CE1 Space Control Register 1 x010 1111 1111 1111
#define CE1_2              (*(volatile ioport int*)0x0807) // EMIF CE1 Space Control Register 2 0100 1111 1111 1111
#define CE1_3              (*(volatile ioport int*)0x0808) // EMIF CE1 Space Control Register 3 xxxx xxxx 0000 0000
#define CE2_1              (*(volatile ioport int*)0x0809) // EMIF CE2 Space Control Register 1 x010 1111 1111 1111
#define CE2_2              (*(volatile ioport int*)0x080A) // EMIF CE2 Space Control Register 2 0101 1111 1111 1111
#define CE2_3              (*(volatile ioport int*)0x080B) // EMIF CE2 Space Control Register 3 xxxx xxxx 0000 0000
#define CE3_1              (*(volatile ioport int*)0x080C) // EMIF CE3 Space Control Register 1 x010 1111 1111 1111
#define CE3_2              (*(volatile ioport int*)0x080D) // EMIF CE3 Space Control Register 2 0101 1111 1111 1111
#define CE3_3              (*(volatile ioport int*)0x080E) // EMIF CE3 Space Control Register 3 xxxx xxxx 0000 0000
#define SDC1               (*(volatile ioport int*)0x080F) // EMIF SDRAM Control Register 1 1111 1001 0100 1000
#define SDPER              (*(volatile ioport int*)0x0810) // EMIF SDRAM Period Register xxxx 0000 1000 0000
#define SDCNT              (*(volatile ioport int*)0x0811) // EMIF SDRAM Counter Register xxxx 0000 1000 0000
#define EMIF_INIT          (*(volatile ioport int*)0x0812) // EMIF SDRAM Init Register xxxx xxxx xxxx xxxx
#define SDC2               (*(volatile ioport int*)0x0813) // EMIF SDRAM Control Register 2 xxxx xx11 1111 1111
#define SDC3               (*(volatile ioport int*)0x0814) // EMIF SDRAM Control Register 3 0000 0000 0000 0111
/*---------------------------------------------------*
 * EGCR register                                     *
 * --------------------------------------------------*/
#define EGCR_MEMFREQ(x)         (((x)&3)<<9)
#define EGCR_WPE                0x0080
#define EGCR_MEMCEN             0x0020
#define EGCR_ARDYOFF            0x0010
#define EGCR_ARDY               0x0008
#define EGCR_HOLD               0x0004
#define EGCR_HOLDA              0x0002
#define EGCR_NOHOLD             0x0001
/*---------------------------------------------------*
 * EMI_BE Register                                   *
 * --------------------------------------------------*/
#define EMI_BE_EMITIME           0x1000
#define EMI_BE_EMICE3            0x0400
#define EMI_BE_EMICE2            0x0200
#define EMI_BE_EMICE1            0x0100
#define EMI_BE_EMICE0            0x0080
#define EMI_BE_EMIDMA            0x0040
#define EMI_BE_EMIFBUS           0x0020
#define EMI_BE_EMIEBUS           0x0010
#define EMI_BE_EMIDBUS           0x0008
#define EMI_BE_EMICBUS           0x0004
#define EMI_BE_EMIPBUS           0x0001
/*---------------------------------------------------*
 * CEn_1, CEn_2, CEn_3 Registers ( n = 1, 2, 3 )     *
 * --------------------------------------------------*/
// CEn_1
#define  CEn_1_MTYPE(x)          (((x)&7)<<12)
#define  CEn_1_READ_SETUP(x)     (((x)&0xF)<<8)
#define  CEn_1_READ_STROBE(x)    (((x)&0x1F)<<2)
#define  CEn_1_READ_HOLD(x)      (((x)&3))
// CEn_2
#define  CEn_2_READ_EXT_HOLD(x)  (((x)&3)<<14)
#define  CEn_2_WRITE_EXT_HOLD(x) (((x)&3)<<12)
#define  CEn_2_WRITE_SETUP(x)    (((x)&0xF)<<8)
#define  CEn_2_WRITE_STROBE(x)   (((x)&0x1F)<<2)
#define  CEn_2_WRITE_HOLD(x)     (((x)&3))
// CEn_3
#define CEn_3_EMITIMEOUT(x)      (x&0xFF)
#if 0
/* --------------------------------------------------*
 * SDC1                                              *
 * --------------------------------------------------*/
#define SDC1
                  .asg "<<11", SDC1_TRC
#define SDC1_4M_16        0x0000
#define SDC1_8M_16        0x0400
#define SDC1_RFEN         0x0100
                  .asg "<<4", SDC1_TRCD
                  .asg " ", SDC1_TRP
/* --------------------------------------------------*
 * SDC2                                              *
 * --------------------------------------------------*/
SDC2_SDACC        .equ 0x4000
                  .asg "<<8", SDC2_TMRD
                  .asg "<<4", SDC2_TRAS
                  .asg " "  , SDC2_TACTV2ACTV
#endif
/*******************************************************************************
 I2C MODULE REGISTERS
 *******************************************************************************/
#define I2CMDR        (*(volatile ioport int*)0x3C09)   // I2C Mode Register
#define I2CIER        (*(volatile ioport int*)0x3C01)   // I2C Interrupt Enable Register
#define I2CSTR        (*(volatile ioport int*)0x3C02)   // I2C Status Register
#define I2CISRC       (*(volatile ioport int*)0x3C0A)   // I2C Interrupt Source Register
#define I2CPSC        (*(volatile ioport int*)0x3C0C)   // I2C Prescaler Register
#define I2CCLKL       (*(volatile ioport int*)0x3C03)   // I2C Clock Divider Low Register
#define I2CCLKH       (*(volatile ioport int*)0x3C04)   // I2C Clock Divider High Register
#define I2CSAR        (*(volatile ioport int*)0x3C07)   // I2C Slave Address Register
#define I2COAR        (*(volatile ioport int*)0x3C00)   // I2C Own Address Register
#define I2CCNT        (*(volatile ioport int*)0x3C05)   // I2C Data Count
#define I2CDRR        (*(volatile ioport int*)0x3C06)   // I2C Data Receive Register
#define I2CDXR        (*(volatile ioport int*)0x3C08)   // I2C Data Transmit Register
#define I2CGPIO       (*(volatile ioport int*)0x3C0B)   // I2C General-Purpose Register
/*-----------------------------------------------------------------------------*
  I2CMDR
 *-----------------------------------------------------------------------------*/
// Bits
#define _I2CMDR_NACKMOD_   15
#define _I2CMDR_FREE_      14
#define _I2CMDR_STT_       13
#define _I2CMDR_IDLEEN_    12
#define _I2CMDR_STP_       11
#define _I2CMDR_MST_       10
#define _I2CMDR_TRX_        9
#define _I2CMDR_XA_         8
#define _I2CMDR_RM_         7
#define _I2CMDR_DLB_        6
#define _I2CMDR_IRS_        5
#define _I2CMDR_STB_        4
#define _I2CMDR_FDF_        3
#define _I2CMDR_BC_         0
// Mask
#define I2CMDR_NACKMOD    0x8000
#define I2CMDR_FREE       0x4000
#define I2CMDR_STT        0x2000
#define I2CMDR_IDLEEN     0x1000
#define I2CMDR_STP        0x0800
#define I2CMDR_MST        0x0400
#define I2CMDR_TRX        0x0200
#define I2CMDR_XA         0x0100
#define I2CMDR_RM         0x0080
#define I2CMDR_DLB        0x0040
#define I2CMDR_IRS        0x0020
#define I2CMDR_STB        0x0010
#define I2CMDR_FDF        0x0008
#define I2CMDR_BC         0x0007
/*-----------------------------------------------------------------------------*
  I2CIER
 *-----------------------------------------------------------------------------*/
// Bits
#define _I2CIER_XRDY_      4
#define _I2CIER_RRDY_      3
#define _I2CIER_ARDY_      2
#define _I2CIER_NACK_      1
#define _I2CIER_AL_        0
// Mask
#define I2CIER_XRDY       0x0010
#define I2CIER_RRDY       0x0008
#define I2CIER_ARDY       0x0004
#define I2CIER_NACK       0x0002
#define I2CIER_AL         0x0001
/*-----------------------------------------------------------------------------*
  I2CSTR
 *-----------------------------------------------------------------------------*/
// Bits
#define _I2CSTR_NACKSNT_  13
#define _I2CSTR_BB_       12
#define _I2CSTR_RSFULL_   11
#define _I2CSTR_XSMT_     10
#define _I2CSTR_AAS_       9
#define _I2CSTR_AD0_       8
#define _I2CSTR_XRDY_      4
#define _I2CSTR_RRDY_      3
#define _I2CSTR_ARDY_      2
#define _I2CSTR_NACK_      1
#define _I2CSTR_AL_        0
// Mask
#define I2CSTR_NACKSNT    0x2000
#define I2CSTR_BB         0x1000
#define I2CSTR_RSFULL     0x0800
#define I2CSTR_XSMT       0x0400
#define I2CSTR_AAS        0x0200
#define I2CSTR_AD0        0x0100
#define I2CSTR_XRDY       0x0010
#define I2CSTR_RRDY       0x0008
#define I2CSTR_ARDY       0x0004
#define I2CSTR_NACK       0x0002
#define I2CSTR_AL         0x0001
/*-----------------------------------------------------------------------------*
 I2CISRC
 *-----------------------------------------------------------------------------*/
// Bits
#define _I2CISRC_TESTMD_   8
#define _I2CISRC_INTCODE_  0
// Mask
#define I2CISRC_TESTMD     0x0F00
#define I2CISRC_INTCODE    0x0007
// I2CISRC INTCODE
#define I2CISRC_INTCODE_XRDY    5
#define I2CISRC_INTCODE_RRDY    4
#define I2CISRC_INTCODE_ARDY    3
#define I2CISRC_INTCODE_NACK    2
#define I2CISRC_INTCODE_AL      1
#define I2CISRC_INTCODE_NONE    0
/*******************************************************************************
 TIMER REGISTERS
 *******************************************************************************/
#define TIM0            (*(volatile ioport int*)0x1000) // Timer0 Count Register
#define PRD0            (*(volatile ioport int*)0x1001) // Timer0 Period Register
#define TCR0            (*(volatile ioport int*)0x1002) // Timer0 Control Register
#define PRSC0           (*(volatile ioport int*)0x1003) // Timer0 Prescaler Register
#define TIM1            (*(volatile ioport int*)0x2400) // Timer1 Count Register
#define PRD1            (*(volatile ioport int*)0x2401) // Timer1 Period Register
#define TCR1            (*(volatile ioport int*)0x2402) // Timer1 Control Register
#define PRSC1           (*(volatile ioport int*)0x2403) // Timer1 Prescaler Register
/*-----------------------------------------------------------------------------*
  TCR
 *-----------------------------------------------------------------------------*/
// Mask
#define TCR_IDLE_EN             0x8000
#define TCR_INT_EXT             0x4000
#define TCR_ERR_TIM             0x2000
#define TCR_FUNC_00             0x0000
#define TCR_FUNC_01             0x0800
#define TCR_FUNC_10             0x1000
#define TCR_FUNC_11             0x1800
#define TCR_TLB                 0x0400
#define TCR_SOFT                0x0200
#define TCR_FREE                0x0100
#define TCR_PWID_8_CPU          0x00C0
#define TCR_PWID_4_CPU          0x0080
#define TCR_PWID_2_CPU          0x0040
#define TCR_PWID_1_CPU          0x0000
#define TCR_ARB                 0x0020
#define TCR_TSS                 0x0010
#define TCR_CP                  0x0008
#define TCR_POLAR               0x0004
#define TCR_DATOUT              0x0002
// Bits
#define _TCR_IDLE_EN_           15
#define _TCR_INT_EXT_           14
#define _TCR_ERR_TIM_           13
#define _TCR_TLB_               10
#define _TCR_SOFT_              9
#define _TCR_FREE_              8
#define _TCR_ARB_               5
#define _TCR_TSS_               4
#define _TCR_C_P_               3
#define _TCR_POLAR_             2
#define _TCR_DATOUT_            1

/*******************************************************************************
 Enable & Disable Interrupts
 *******************************************************************************/
#define ENABLE_INTERRUPT()       asm("     BCLR ST1_INTM")
#define DISABLE_INTERRUPT()      asm("     BSET ST1_INTM")

#define ENABLE_TIMER1_INT()      asm("     OR #1<<6, *(IER1)")
#define DISABLE_TIMER1_INT()     asm("     AND #~(1<<6), *(IER1)")

#define ENABLE_INT0_INT()        asm("     OR #0x0004, *(IER0)")
#define DISABLE_INT0_INT()       asm("     AND #~(0x0004), *(IER0)")

#define ENABLE_DMAC5_INT()       asm("     OR #0x8000, *(IER0)")
#define DISABLE_DMAC5_INT()      asm("     AND #~(0x8000), *(IER0)")

#define ENABLE_DMAC4_INT()       asm("     OR #0x4000, *(IER0)")
#define DISABLE_DMAC4_INT()      asm("     AND #~(0x4000), *(IER0)")

#define ENABLE_XINT2_INT()       asm("     OR #0x2000, *(IER0)")
#define DISABLE_XINT2_INT()      asm("     AND #~(0x2000), *(IER0)")

#define ENABLE_MMCSD2_INT()      asm("     OR #0x2000, *(IER0)")
#define DISABLE_MMCSD2_INT()     asm("     AND #~(0x2000), *(IER0)")

#define ENABLE_RINT2_INT()       asm("     OR #0x1000, *(IER0)")
#define DISABLE_RINT2_INT()      asm("     AND #~(0x1000), *(IER0)")

#define ENABLE_INT3_INT()        asm("     OR #0x0800, *(IER0)")
#define DISABLE_INT3_INT()       asm("     AND #~(0x0800), *(IER0)")

#define ENABLE_WDTINT_INT()      asm("     OR #0x0800, *(IER0)")
#define DISABLE_WDTINT_INT()     asm("     AND #~(0x0800), *(IER0)")

#define ENABLE_DSPINT_INT()      asm("     OR #0x0400, *(IER0)")
#define DISABLE_DSPINT_INT()     asm("     AND #~(0x0400), *(IER0)")

#define ENABLE_DMAC1_INT()       asm("     OR #0x0200, *(IER0)")
#define DISABLE_DMAC1_INT()      asm("     AND #~(0x0200), *(IER0)")

#define ENABLE_USB_INT()         asm("     OR #0x0100, *(IER0)")
#define DISABLE_USB_INT()        asm("     AND #~(0x0100), *(IER0)")

#define ENABLE_XINT1_INT()       asm("     OR #0x0080, *(IER0)")
#define DISABLE_XINT1_INT()      asm("     AND #~(0x0080), *(IER0)")

#define ENABLE_MMCSD1_INT()      asm("     OR #0x0080, *(IER0)")
#define DISABLE_MMCSD1_INT()     asm("     AND #~(0x0080), *(IER0)")

#define ENABLE_RINT1_INT()       asm("     OR #0x0040, *(IER0)")
#define DISABLE_RINT1_INT()      asm("     AND #~(0x0040), *(IER0)")

#define ENABLE_RINT0_INT()       asm("     OR #0x0020, *(IER0)")
#define DISABLE_RINT0_INT()      asm("     AND #~(0x0020), *(IER0)")

#define ENABLE_TINT0_INT()       asm("     OR #0x0010, *(IER0)")
#define DISABLE_TINT0_INT()      asm("     AND #~(0x0010), *(IER0)")

#define ENABLE_INT2_INT()       asm("     OR #0x0008, *(IER0)")
#define DISABLE_INT2_INT()      asm("     AND #~(0x0008), *(IER0)")

#define ENABLE_INT0_INT()       asm("     OR #0x0004, *(IER0)")
#define DISABLE_INT0_INT()      asm("     AND #~(0x0004), *(IER0)")

//------------------------------------------------------------------------------
#define ENABLE_RTOS_INT()       asm("     OR #0x0400, *(IER1)")
#define DISABLE_RTOS_INT()      asm("     AND #~(0x0400), *(IER1)")

#define ENABLE_DLOG_INT()       asm("     OR #0x0200, *(IER1)")
#define DISABLE_DLOG_INT()      asm("     AND #~(0x0200), *(IER1)")

#define ENABLE_BERR_INT()       asm("     OR #0x0100, *(IER1)")
#define DISABLE_BERR_INT()      asm("     AND #~(0x0100), *(IER1)")

#define ENABLE_I2C_INT()        asm("     OR #0x0080, *(IER1)")
#define DISABLE_I2C_INT()       asm("     AND #~(0x0080), *(IER1)")

#define ENABLE_TINT1_INT()      asm("     OR #0x0040, *(IER1)")
#define DISABLE_TINT1_INT()     asm("     AND #~(0x0040), *(IER1)")

#define ENABLE_DMAC3_INT()      asm("     OR #0x0020, *(IER1)")
#define DISABLE_DMAC3_INT()     asm("     AND #~(0x0020), *(IER1)")
#define CLEAR_DMAC3_INT()       asm("     OR #0x0020, *(IFR1)");

#define ENABLE_DMAC2_INT()      asm("     OR #0x0010, *(IER1)")
#define DISABLE_DMAC2_INT()     asm("     AND #~(0x0010), *(IER1)")
#define CLEAR_DMAC2_INT()       asm("     OR #0x0010, *(IFR1)");

#define ENABLE_INT4_INT()       asm("     OR #0x0008, *(IER1)")
#define DISABLE_INT4_INT()      asm("     AND #~(0x0008), *(IER1)")

#define ENABLE_RTC_INT()        asm("     OR #0x0008, *(IER1)")
#define DISABLE_RTC_INT()       asm("     AND #~(0x0008), *(IER1)")

#define ENABLE_DMAC0_INT()      asm("     OR #0x0004, *(IER1)")
#define DISABLE_DMAC0_INT()     asm("     AND #~(0x0004), *(IER1)")

#define ENABLE_XINT0_INT()      asm("     OR #0x0002, *(IER1)")
#define DISABLE_XINT0_INT()     asm("     AND #~(0x0002), *(IER1)")

#define ENABLE_INT1_INT()       asm("     OR #0x0001, *(IER1)")
#define DISABLE_INT1_INT()      asm("     AND #~(0x0001), *(IER1)")
/*******************************************************************************
 Vector Interrupts
 *******************************************************************************/
#define RESET_INT_VEC_NO         0
#define SINT0                    0 // Reset (hardware and software)

#define NMI_INT_VEC_NO           1
#define SINT1                    1 // Nonmaskable interrupt

#define BERR_INT_VEC_NO         24
#define SINT24                  24 // Bus Error interrupt

#define INT0_INT_VEC_NO          2
#define SINT2                    2 // External interrupt #0

#define INT1_INT_VEC_NO         16
#define SINT16                  16 // External interrupt #1

#define INT2_INT_VEC_NO          3
#define SINT3                    3 // External interrupt #2

#define TINT0_INT_VEC_NO         4
#define SINT4                    4 // Timer #0 interrupt

#define RINT0_INT_VEC_NO         5
#define SINT5                    5 // McBSP #0 receive interrupt

#define XINT0_INT_VEC_NO        17
#define SINT17                  17 // McBSP #0 transmit interrupt

#define RINT1_INT_VEC_NO         6
#define SINT6                    6 // McBSP #1 receive interrupt

#define XINT1_INT_VEC_NO         7
#define MMCSD1_INT_VEC_NO        7
#define SINT7                    7 // McBSP #1 transmit interrupt, MMC/SD #1 interrupt

#define USB_INT_VEC_NO           8
#define SINT8                    8 // USB interrupt

#define DMAC0_INT_VEC_NO        18
#define SINT18                  18 // DMA Channel #0 interrupt

#define DMAC1_INT_VEC_NO         9
#define SINT9                    9 // DMA Channel #1 interrupt

#define DSPINT_INT_VEC_NO       10
#define SINT10                  10 // Interrupt from host

#define INT3_INT_VEC_NO         11
#define WDTINT_INT_VEC_NO       11
#define SINT11                  11 // External interrupt #3 or Watchdog timer interrupt

#define INT4_INT_VEC_NO         19
#define RTC_INT_VEC_NO          19
#define SINT19                  19 // External interrupt #4 or RTC interrupt

#define RINT2_INT_VEC_NO        12
#define SINT12                  12 // McBSP #2 receive interrupt

#define XINT2_INT_VEC_NO        13
#define MMCSD2_INT_VEC_NO       13
#define SINT13                  13 // McBSP #2 transmit interrupt , MMC/SD #2 interrupt

#define DMAC2_INT_VEC_NO        20
#define SINT20                  20 // DMA Channel #2 interrupt

#define DMAC3_INT_VEC_NO        21
#define SINT21                  21 // DMA Channel #3 interrupt

#define DMAC4_INT_VEC_NO        14
#define SINT14                  14 // DMA Channel #4 interrupt

#define DMAC5_INT_VEC_NO        15
#define SINT15                  15 // DMA Channel #5 interrupt

#define TINT1_INT_VEC_NO        22
#define SINT22                  22 //  Timer #1 interrupt

#define I2C_INT_VEC_NO          23
#define IIC_INT_VEC_NO          23
#define SINT23                  23 // I2C interrupt

#define DLOG_INT_VEC_NO         25
#define SINT25                  25 // Data Log interrupt

#define RTOS_INT_VEC_NO         26
#define SINT26                  26 // Real-time Operating System interrupt

#define SINT27                  27 // Software interrupt #27
#define SINT28                  28 // Software interrupt #28
#define SINT29                  29 // Software interrupt #29
#define SINT30                  30 // Software interrupt #30
#define SINT31                  31 // Software interrupt #31

/*******************************************************************************
 * USB Module
 *******************************************************************************/
// USB PLL Clock Generator
#define USBCLKMD        (*(volatile ioport int*)0x1E00)
// USBCLKMD
#define USBCLKMD_PLL_ENABLE   0x10
#define USBCLKMD_PLL_DIV(d)   ((d)<<5)
#define USBCLKMD_PLL_MULT(m)  ((m)<<7)
/*
// DMA CONTEXT
// 0x5800 Reserved
DMAC_O1                    .equ 0x5808 ;Output Endpoint 1 DMA Context
DMAC_O2                    .equ 0x5810 ;Output Endpoint 2 DMA Context
DMAC_O3                    .equ 0x5818 ;Output Endpoint 3 DMA Context
DMAC_O4                    .equ 0x5820 ;Output Endpoint 4 DMA Context
DMAC_O5                    .equ 0x5828 ;Output Endpoint 5 DMA Context
DMAC_O6                    .equ 0x5830 ;Output Endpoint 6 DMA Context
DMAC_O7                    .equ 0x5838 ;Output Endpoint 7 DMA Context
//0x5840 Reserved
DMAC_I1                    0x5848 // Input Endpoint 1 DMA Context
DMAC_I2                    0x5850 // Input Endpoint 2 DMA Context
DMAC_I3                    0x5858 // Input Endpoint 3 DMA Context
DMAC_I4                    0x5860 // Input Endpoint 4 DMA Context
DMAC_I5                    0x5868 // Input Endpoint 5 DMA Context
DMAC_I6                    0x5870 // Input Endpoint 6 DMA Context
DMAC_I7                    0x5878 // Input Endpoint 7 DMA Context
USB_DATA_BUFF              0x5880 // Contains X/Y data buffers for endpoints 1 - 7
// BUFFER RAM
OEB_0                      0x6680 // Output Endpoint 0 Buffer
IEB_0                      0x66C0 // Input Endpoint 0 Buffer
SUP_0                      0x6700 // Setup Packet for Endpoint
OEDB_1                     0x6708 // Output Endpoint 1 Descriptor Block
OEDB_2                     0x6710 // Output Endpoint 2 Descriptor Block
OEDB_3                     0x6718 // Output Endpoint 3 Descriptor Block
OEDB_4                     0x6720 // Output Endpoint 4 Descriptor Block
OEDB_5                     0x6728 // Output Endpoint 5 Descriptor Block
OEDB_6                     0x6730 // Output Endpoint 6 Descriptor Block
OEDB_7                     0x6738 // Output Endpoint 7 Descriptor Block
// 0x6740 Reserved
IEDB_1                     0x6748 // Input Endpoint 1 Descriptor Block
IEDB_2                     0x6750 // Input Endpoint 2 Descriptor Block
IEDB_3                     0x6758 // Input Endpoint 3 Descriptor Block
IEDB_4                     0x6760 // Input Endpoint 4 Descriptor Block
IEDB_5                     0x6768 // Input Endpoint 5 Descriptor Block
IEDB_6                     0x6770 // Input Endpoint 6 Descriptor Block
IEDB_7                     0x6778 // Input Endpoint 7 Descriptor Block
// CONTROL AND STATUS REGISTERS
IEPCNF_0                   0x6780 Input Endpoint 0 Configuration
IEPBCNT_0                  0x6781 Input Endpoint 0 Byte Count
OEPCNF_0                   0x6782 Output Endpoint 0 Configuration
OEPBCNT_0                  0x6783 Output Endpoint 0 Byte Count
GLOBCTL                    0x6791 Global Control Register
VECINT                     0x6792 Vector Interrupt Register
IEPINT                     0x6793 Input Endpoint Interrupt Register
OEPINT                     0x6794 Output Endpoint Interrupt Register
IDMARINT                   0x6795 Input DMA Reload Interrupt Register
ODMARINT                   0x6796 Output DMA Reload Interrupt Register
IDMAGINT                   0x6797 Input DMA Go Interrupt Register
ODMAGINT                   0x6798 Output DMA Go Interrupt Register
IDMAMSK                    0x6799 Input DMA Interrupt Mask Register
ODMAMSK                    0x679A Output DMA Interrupt Mask Register
IEDBMSK                    0x679B Input EDB Interrupt Mask Register
OEDBMSK                    0x679C Output EDB Interrupt Mask Register
HOSTCTL                    0x67A0 Host DMA Control Register
HOSTEP                     0x67A1 Host DMA Endpoint Register
HOST                       0x67A2 Host DMA Status
FNUML                      0x67F8 Frame Number Low Register
FNUMH                      0x67F9 Frame Number High
PSOFTMR                    0x67FA PreSOF Interrupt Timer Register
//USBCTL                     0x67FC ;USB Control Register
USBMSK                     0x67FD USB Interrupt Mask Register
USBSTA                     0x67FE USB Status Register
;
USB_IDLE                   0x7000 USB idle control
*/

// Defined by spru317
#define USBREGBASE      ( (volatile ioport int*)0x5800) // Base addres of USB module registers
#define USBICNF0        (*(volatile ioport int*)0x6780) // Configuration Register for IN Endpoint 0
#define USBICT0         (*(volatile ioport int*)0x6781) // Count Register for IN Endpoint 0
#define USBOCNF0        (*(volatile ioport int*)0x6782) // Configuration Register for OUT Endpoint 0
#define USBOCT0         (*(volatile ioport int*)0x6783) // Count Register for OUT Endpoint 0
#define USBGCTL         (*(volatile ioport int*)0x6791) // Global Control Register

#define USBINTSRC       (*(volatile ioport int*)0x6792) // Interrupt Source Register
#define USBIEPIF        (*(volatile ioport int*)0x6793) // Endpoint Interrupt Flag Register for OUT Endpoints
#define USBOEPIF        (*(volatile ioport int*)0x6794) // Endpoint Interrupt Flag Register for IN Endpoints

#define USBIEPIE        (*(volatile ioport int*)0x679B) // Endpoint Interrupt Enable Register for IN Endpoints
#define USBOEPIE        (*(volatile ioport int*)0x679C) // Endpoint Interrupt Enable Register for OUT Endpoints

#define USBCTL          (*(volatile ioport int*)0x67FC) // USB Control Register
#define USBIE           (*(volatile ioport int*)0x67FD) // USB Interrupt Enable Register
#define USBIF           (*(volatile ioport int*)0x67FE) // USB Interrupt Flag Register
#define USBIDLECTL      (*(volatile ioport int*)0x7000) // USB idle control
#define USB_EP_DATA_BUF (*(volatile ioport int*)0x5880) // Contains X/Y data buffers for endpoints 1 - 7
#define USB_SETUP_PACKET (*(volatile ioport int*)0x6700) // Setup Packet for Endpoint
#define USB_OEB_0       (*(volatile ioport int*)0x6680) // Output Endpoint 0 Buffer
#define USB_IEB_0       (*(volatile ioport int*)0x66C0) // Input Endpoint 0 Buffer
#define USBADDR         (*(volatile ioport int*)0x67FF) // USB Device Address Register
#define USBFNUML        (*(volatile ioport int*)0x67F8) // Frame Number Low
#define USBFNUMH        (*(volatile ioport int*)0x67F9) // Frame Number High

//------------------------------------------------------------------------------
// DMA context registers
//------------------------------------------------------------------------------
// addr = base + n*8
// Control register
#define USBODCTL(n)     (*(volatile ioport int*)(0x5800+(n)*8))
#define USBIDCTL(n)     (*(volatile ioport int*)(0x5840+(n)*8))
// Size register (transfer size in bytes)
#define USBODSIZ(n)     (*(volatile ioport int*)(0x5801+(n)*8))
#define USBIDSIZ(n)     (*(volatile ioport int*)(0x5841+(n)*8))
// Address register, low part (byte address for a location in DSP memory)
#define USBODADL(n)     (*(volatile ioport int*)(0x5802+(n)*8))
#define USBIDADL(n)     (*(volatile ioport int*)(0x5842+(n)*8))
// Address register, high part (byte address for a location in DSP memory)
#define USBODADH(n)     (*(volatile ioport int*)(0x5803+(n)*8))
#define USBIDADH(n)     (*(volatile ioport int*)(0x5843+(n)*8))
// Count register (transfer count in bytes)
#define USBODCT(n)      (*(volatile ioport int*)(0x5804+(n)*8))
#define USBIDCT(n)      (*(volatile ioport int*)(0x5844+(n)*8))
// Reload-size register (reload value for USBxDSIZ, x = O or I)
#define USBODRSZ(n)     (*(volatile ioport int*)(0x5805+(n)*8))
#define USBIDRSZ(n)     (*(volatile ioport int*)(0x5845+(n)*8))
// Reload-address register, low part (reload value for USBxDADL, x = O or I)
#define USBODRAL(n)     (*(volatile ioport int*)(0x5806+(n)*8))
#define USBIDRAL(n)     (*(volatile ioport int*)(0x5846+(n)*8))
// Reload-address register, high part (reload value for USBxDADH, x = O or I)
#define USBODRAH(n)     (*(volatile ioport int*)(0x5807+(n)*8))
#define USBIDRAH(n)     (*(volatile ioport int*)(0x5847+(n)*8))
//------------------------------------------------------------------------------
// USBDCTL
//------------------------------------------------------------------------------
#define USBDCTL_PM      0x0100
#define USBDCTL_EM      0x0080
#define USBDCTL_SHT     0x0040
#define USBDCTL_CAT     0x0020
#define USBDCTL_END     0x0010
#define USBDCTL_OVF     0x0008
#define USBDCTL_RLD     0x0004
#define USBDCTL_STP     0x0002
#define USBDCTL_GO      0x0001

#define _USBDCTL_PM     8
#define _USBDCTL_GO     0
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
// DMA RLD (reload) interrupt flag register for IN endpoints
#define USBIDRIF         (*(volatile ioport int*)0x6795)
// DMA RLD (reload) interrupt flag register for OUT endpoints
#define USBODRIF         (*(volatile ioport int*)0x6796)
// DMA GO interrupt flag register for IN endpoints
#define USBIDGIF         (*(volatile ioport int*)0x6797)
// DMA GO interrupt flag register for OUT endpoints
#define USBODGIF         (*(volatile ioport int*)0x6798)
// DMA interrupt enable register for IN endpoints
#define USBIDIE          (*(volatile ioport int*)0x6799)
// DMA interrupt enable register for OUT endpoints
#define USBODIE          (*(volatile ioport int*)0x679A)
//------------------------------------------------------------------------------
// USBINTSRC
//------------------------------------------------------------------------------
#define USBINTSRC_NO_INT           0x00
#define USBINTSRC_OUT_EP_0         0x02
#define USBINTSRC_IN_EP_0          0x04
#define USBINTSRC_RSTR             0x06
#define USBINTSRC_SUSR             0x08
#define USBINTSRC_RESR             0x0A
#define USBINTSRC_SETUP            0x0C
#define USBINTSRC_STPOW            0x0E
#define USBINTSRC_SOF              0x10
#define USBINTSRC_PSOF             0x11

#define USBINTSRC_OUT_EP(n)        (((n)-1)*2 + 0x12)

#define USBINTSRC_OUT_EP_1         0x12
#define USBINTSRC_OUT_EP_2         0x14
#define USBINTSRC_OUT_EP_3         0x16
#define USBINTSRC_OUT_EP_4         0x18
#define USBINTSRC_OUT_EP_5         0x1A
#define USBINTSRC_OUT_EP_6         0x1C
#define USBINTSRC_OUT_EP_7         0x1E

#define USBINTSRC_IN_EP(n)        (((n)-1)*2 + 0x22)

#define USBINTSRC_IN_EP_1          0x22
#define USBINTSRC_IN_EP_2          0x24
#define USBINTSRC_IN_EP_3          0x26
#define USBINTSRC_IN_EP_4          0x28
#define USBINTSRC_IN_EP_5          0x2A
#define USBINTSRC_IN_EP_6          0x2C
#define USBINTSRC_IN_EP_7          0x2E
#define USBINTSRC_OUT_EP_1_DMA_RELOAD 0x32
#define USBINTSRC_OUT_EP_1_DMA_GO     0x33
#define USBINTSRC_OUT_EP_2_DMA_RELOAD 0x34
#define USBINTSRC_OUT_EP_2_DMA_GO     0x35
#define USBINTSRC_OUT_EP_3_DMA_RELOAD 0x36
#define USBINTSRC_OUT_EP_3_DMA_GO     0x37
#define USBINTSRC_OUT_EP_4_DMA_RELOAD 0x38
#define USBINTSRC_OUT_EP_4_DMA_GO     0x39
#define USBINTSRC_OUT_EP_5_DMA_RELOAD 0x3A
#define USBINTSRC_OUT_EP_5_DMA_GO     0x3B
#define USBINTSRC_OUT_EP_6_DMA_RELOAD 0x3C
#define USBINTSRC_OUT_EP_6_DMA_GO     0x3D
#define USBINTSRC_OUT_EP_7_DMA_RELOAD 0x3E
#define USBINTSRC_OUT_EP_7_DMA_GO     0x3F
#define USBINTSRC_IN_EP_1_DMA_RELOAD  0x42
#define USBINTSRC_IN_EP_1_DMA_GO      0x43
#define USBINTSRC_IN_EP_2_DMA_RELOAD  0x44
#define USBINTSRC_IN_EP_2_DMA_GO      0x45
#define USBINTSRC_IN_EP_3_DMA_RELOAD  0x46
#define USBINTSRC_IN_EP_3_DMA_GO      0x47
#define USBINTSRC_IN_EP_4_DMA_RELOAD  0x48
#define USBINTSRC_IN_EP_4_DMA_GO      0x49
#define USBINTSRC_IN_EP_5_DMA_RELOAD  0x4A
#define USBINTSRC_IN_EP_5_DMA_GO      0x4B
#define USBINTSRC_IN_EP_6_DMA_RELOAD  0x4C
#define USBINTSRC_IN_EP_6_DMA_GO      0x4D
#define USBINTSRC_IN_EP_7_DMA_RELOAD  0x4E
#define USBINTSRC_IN_EP_7_DMA_GO      0x4F
#define USBINTSRC_HOST_INT            0x50
#define USBINTSRC_HOST_ERROR          0x52
//------------------------------------------------------------------------------
// USBCTL
//------------------------------------------------------------------------------
#define USBCTL_CONN        0x80
#define USBCTL_FEN         0x40
#define USBCTL_RWUP        0x20
#define USBCTL_FRSTE       0x10
#define USBCTL_SETUP       0x02
#define USBCTL_DIR         0x01
//------------------------------------------------------------------------------
// USBIDLECTL
//------------------------------------------------------------------------------
#define USBIDLECTL_USBRST    0x04
#define USBIDLECTL_IDLESTAT  0x02
#define USBIDLECTL_IDLEEN    0x01
//------------------------------------------------------------------------------
// USBxCNFn
//------------------------------------------------------------------------------
#define USBCNF_UBME         0x80
#define USBCNF_ISO          0x40
#define USBCNF_TOGGLE       0x20
#define USBCNF_DBUF         0x10
#define USBCNF_STALL        0x08
#define USBCNF_CTXH(h)      ((h)<<3)
#define USBCNF_CTYH(h)       (h)
//------------------------------------------------------------------------------
// USBxCTn
//------------------------------------------------------------------------------
#define USBCT_NACK          0x80
//------------------------------------------------------------------------------
// USB Interrupt Enable Register (USBIE)
//------------------------------------------------------------------------------
#define USBIE_RSTR          0x80
#define USBIE_SUSR          0x40
#define USBIE_RESR          0x20
#define USBIE_SOF           0x10
#define USBIE_PSOF          0x08
#define USBIE_SETUP         0x04
#define USBIE_STPOW         0x01
//------------------------------------------------------------------------------
// USB Interrupt Flag Register (USBIF)
//------------------------------------------------------------------------------
#define USBIF_RSTR          0x80
#define USBIF_SUSR          0x40
#define USBIF_RESR          0x20
#define USBIF_SOF           0x10
#define USBIF_PSOF          0x08
#define USBIF_SETUP         0x04
#define USBIF_STPOW         0x01

#define USB_EP_DIR_IN       0x80
#define USB_EP_DIR_OUT      0x00
//------------------------------------------------------------------------------
// Descriptor Registers for Each OUT or IN Endpoint(1-7)
//------------------------------------------------------------------------------
// Endpoint n configuration register
#define USBOCNF(n)          (*(volatile ioport int*)(0x6700+(n)*8))
#define USBICNF(n)          (*(volatile ioport int*)(0x6740+(n)*8))
// X-buffer base address register (bits 11-4 of a byte address)
#define USBOBAX(n)          (*(volatile ioport int*)(0x6701+(n)*8))
#define USBIBAX(n)          (*(volatile ioport int*)(0x6741+(n)*8))
// X-buffer count register (transfer count in bytes)
#define USBOCTX(n)          (*(volatile ioport int*)(0x6702+(n)*8))
#define USBICTX(n)          (*(volatile ioport int*)(0x6742+(n)*8))
// X-buffer count extension register (used for isochronous transfers only)
#define USBOCTXH(n)         (*(volatile ioport int*)(0x6703+(n)*8))
// X-/Y-buffer size extension register (used for isochronous transfers only)
#define USBISIZH(n)         (*(volatile ioport int*)(0x6743+(n)*8))
// X-/Y-buffer size register (transfer size in bytes)
#define USBOSIZ(n)          (*(volatile ioport int*)(0x6704+(n)*8))
#define USBISIZ(n)          (*(volatile ioport int*)(0x6744+(n)*8))
// Y-buffer base address register (bits 11-4 of a byte address)
#define USBOBAY(n)          (*(volatile ioport int*)(0x6705+(n)*8))
#define USBIBAY(n)          (*(volatile ioport int*)(0x6745+(n)*8))
// Y-buffer count register (transfer count in bytes)
#define USBOCTY(n)          (*(volatile ioport int*)(0x6706+(n)*8))
#define USBICTY(n)          (*(volatile ioport int*)(0x6746+(n)*8))
// Y-buffer count extension register (used for isochronous transfers only)
#define USBOCTYH(n)         (*(volatile ioport int*)(0x6707+(n)*8))
//------------------------------------------------------------------------------
/*******************************************************************************
 GPIO module
 *******************************************************************************/
#define GPIO_IODIR     (*(volatile ioport int*)0x3400) // General-purpose I/O Register
#define GPIO_IODATA    (*(volatile ioport int*)0x3401) // General-purpose I/O Data Register
#define GPIO_AGPIOEN   (*(volatile ioport int*)0x4400) // Address/GPIO Enable Register
#define GPIO_AGPIODIR  (*(volatile ioport int*)0x4401) // Address/GPIO Direction Register
#define GPIO_AGPIODATA (*(volatile ioport int*)0x4402) // Address/GPIO Data Register
/*******************************************************************************
 MCBSP module
 *******************************************************************************/
#define DRR2  0x00
#define DRR1  0x01
#define DXR2  0x02
#define DXR1  0x03
#define SPCR2 0x04
#define SPCR1 0x05
#define RCR2  0x06
#define RCR1  0x07
#define XCR2  0x08
#define XCR1  0x09
#define SRGR2 0x0A
#define SRGR1 0x0B
#define MCR2  0x0C
#define MCR1  0x0D
#define RCERA 0x0E
#define RCERB 0x0F
#define XCERA 0x10
#define XCERB 0x11
#define PCR   0x12
#define RCERC 0x13
#define RCERD 0x14
#define XCERC 0x15
#define XCERD 0x16
#define RCERE 0x17
#define RCERF 0x18
#define XCERE 0x19
#define XCERF 0x1A
#define RCERG 0x1B
#define RCERH 0x1C
#define XCERG 0x1D
#define XCERH 0x1E
//
#define MCBSP0(r)    (*(volatile ioport int*)(0x2800 + (r)))
#define MCBSP1(r)    (*(volatile ioport int*)(0x2C00 + (r)))
#define MCBSP2(r)    (*(volatile ioport int*)(0x3000 + (r)))
// MCBSP0
#define MCBSP0_DRR2  (*(volatile ioport int*)0x2800) // Data Receive Register 2
#define MCBSP0_DRR1  (*(volatile ioport int*)0x2801) // Data Receive Register 1
#define MCBSP0_DXR2  (*(volatile ioport int*)0x2802) // Data Transmit Register 2
#define MCBSP0_DXR1  (*(volatile ioport int*)0x2803) // Data Transmit Register 1
#define MCBSP0_SPCR2 (*(volatile ioport int*)0x2804) // Serial Port Control Register 2
#define MCBSP0_SPCR1 (*(volatile ioport int*)0x2805) // Serial Port Control Register 1
#define MCBSP0_RCR2  (*(volatile ioport int*)0x2806) // Receive Control Register 2
#define MCBSP0_RCR1  (*(volatile ioport int*)0x2807) // Receive Control Register 1
#define MCBSP0_XCR2  (*(volatile ioport int*)0x2808) // Transmit Control Register 2
#define MCBSP0_XCR1  (*(volatile ioport int*)0x2809) // Transmit Control Register 1
#define MCBSP0_SRGR2 (*(volatile ioport int*)0x280A) // Sample Rate Generator Register 2
#define MCBSP0_SRGR1 (*(volatile ioport int*)0x280B) // Sample Rate Generator Register 1
#define MCBSP0_MCR2  (*(volatile ioport int*)0x280C) // Multichannel Register 2
#define MCBSP0_MCR1  (*(volatile ioport int*)0x280D) // Multichannel Register 1
#define MCBSP0_RCERA (*(volatile ioport int*)0x280E) // Receive Channel Enable Register Partition A
#define MCBSP0_RCERB (*(volatile ioport int*)0x280F) // Receive Channel Enable Register Partition B
#define MCBSP0_XCERA (*(volatile ioport int*)0x2810) // Transmit Channel Enable Register Partition A
#define MCBSP0_XCERB (*(volatile ioport int*)0x2811) // Transmit Channel Enable Register Partition B
#define MCBSP0_PCR   (*(volatile ioport int*)0x2812) // Pin Control Register
#define MCBSP0_RCERC (*(volatile ioport int*)0x2813) // Receive Channel Enable Register Partition C
#define MCBSP0_RCERD (*(volatile ioport int*)0x2814) // Receive Channel Enable Register Partition D
#define MCBSP0_XCERC (*(volatile ioport int*)0x2815) // Transmit Channel Enable Register Partition C
#define MCBSP0_XCERD (*(volatile ioport int*)0x2816) // Transmit Channel Enable Register Partition D
#define MCBSP0_RCERE (*(volatile ioport int*)0x2817) // Receive Channel Enable Register Partition E
#define MCBSP0_RCERF (*(volatile ioport int*)0x2818) // Receive Channel Enable Register Partition F
#define MCBSP0_XCERE (*(volatile ioport int*)0x2819) // Transmit Channel Enable Register Partition E
#define MCBSP0_XCERF (*(volatile ioport int*)0x281A) // Transmit Channel Enable Register Partition F
#define MCBSP0_RCERG (*(volatile ioport int*)0x281B) // Receive Channel Enable Register Partition G
#define MCBSP0_RCERH (*(volatile ioport int*)0x281C) // Receive Channel Enable Register Partition H
#define MCBSP0_XCERG (*(volatile ioport int*)0x281D) // Transmit Channel Enable Register Partition G
#define MCBSP0_XCERH (*(volatile ioport int*)0x281E) // Transmit Channel Enable Register Partition H
// MCBSP1
#define MCBSP1_DRR2  (*(volatile ioport int*)0x2C00) //  Data Receive Register 2
#define MCBSP1_DRR1  (*(volatile ioport int*)0x2C01) //  Data Receive Register 1
#define MCBSP1_DXR2  (*(volatile ioport int*)0x2C02) //  Data Transmit Register 2
#define MCBSP1_DXR1  (*(volatile ioport int*)0x2C03) //  Data Transmit Register 1
#define MCBSP1_SPCR2 (*(volatile ioport int*)0x2C04) //  Serial Port Control Register 2
#define MCBSP1_SPCR1 (*(volatile ioport int*)0x2C05) //  Serial Port Control Register 1
#define MCBSP1_RCR2  (*(volatile ioport int*)0x2C06) //  Receive Control Register 2
#define MCBSP1_RCR1  (*(volatile ioport int*)0x2C07) //  Receive Control Register 1
#define MCBSP1_XCR2  (*(volatile ioport int*)0x2C08) //  Transmit Control Register 2
#define MCBSP1_XCR1  (*(volatile ioport int*)0x2C09) //  Transmit Control Register 1
#define MCBSP1_SRGR2 (*(volatile ioport int*)0x2C0A) //  Sample Rate Generator Register 2
#define MCBSP1_SRGR1 (*(volatile ioport int*)0x2C0B) //  Sample Rate Generator Register 1
#define MCBSP1_MCR2  (*(volatile ioport int*)0x2C0C) //  Multichannel Register 2
#define MCBSP1_MCR1  (*(volatile ioport int*)0x2C0D) //  Multichannel Register 1
#define MCBSP1_RCERA (*(volatile ioport int*)0x2C0E) //  Receive Channel Enable Register Partition A
#define MCBSP1_RCERB (*(volatile ioport int*)0x2C0F) //  Receive Channel Enable Register Partition B
#define MCBSP1_XCERA (*(volatile ioport int*)0x2C10) //  Transmit Channel Enable Register Partition A
#define MCBSP1_XCERB (*(volatile ioport int*)0x2C11) //  Transmit Channel Enable Register Partition B
#define MCBSP1_PCR   (*(volatile ioport int*)0x2C12) //  Pin Control Register
#define MCBSP1_RCERC (*(volatile ioport int*)0x2C13) //  Receive Channel Enable Register Partition C
#define MCBSP1_RCERD (*(volatile ioport int*)0x2C14) //  Receive Channel Enable Register Partition D
#define MCBSP1_XCERC (*(volatile ioport int*)0x2C15) //  Transmit Channel Enable Register Partition C
#define MCBSP1_XCERD (*(volatile ioport int*)0x2C16) //  Transmit Channel Enable Register Partition D
#define MCBSP1_RCERE (*(volatile ioport int*)0x2C17) //  Receive Channel Enable Register Partition E
#define MCBSP1_RCERF (*(volatile ioport int*)0x2C18) //  Receive Channel Enable Register Partition F
#define MCBSP1_XCERE (*(volatile ioport int*)0x2C19) //  Transmit Channel Enable Register Partition E
#define MCBSP1_XCERF (*(volatile ioport int*)0x2C1A) //  Transmit Channel Enable Register Partition F
#define MCBSP1_RCERG (*(volatile ioport int*)0x2C1B) //  Receive Channel Enable Register Partition G
#define MCBSP1_RCERH (*(volatile ioport int*)0x2C1C) //  Receive Channel Enable Register Partition H
#define MCBSP1_XCERG (*(volatile ioport int*)0x2C1D) //  Transmit Channel Enable Register Partition G
#define MCBSP1_XCERH (*(volatile ioport int*)0x2C1E) //  Transmit Channel Enable Register Partition H
// MCBSP2
#define MCBSP2_DRR2  (*(volatile ioport int*)0x3000) //  Data Receive Register 2
#define MCBSP2_DRR1  (*(volatile ioport int*)0x3001) //  Data Receive Register 1
#define MCBSP2_DXR2  (*(volatile ioport int*)0x3002) //  Data Transmit Register 2
#define MCBSP2_DXR1  (*(volatile ioport int*)0x3003) //  Data Transmit Register 1
#define MCBSP2_SPCR2 (*(volatile ioport int*)0x3004) //  Serial Port Control Register 2
#define MCBSP2_SPCR1 (*(volatile ioport int*)0x3005) //  Serial Port Control Register 1
#define MCBSP2_RCR2  (*(volatile ioport int*)0x3006) //  Receive Control Register 2
#define MCBSP2_RCR1  (*(volatile ioport int*)0x3007) //  Receive Control Register 1
#define MCBSP2_XCR2  (*(volatile ioport int*)0x3008) //  Transmit Control Register 2
#define MCBSP2_XCR1  (*(volatile ioport int*)0x3009) //  Transmit Control Register 1
#define MCBSP2_SRGR2 (*(volatile ioport int*)0x300A) //  Sample Rate Generator Register 2
#define MCBSP2_SRGR1 (*(volatile ioport int*)0x300B) //  Sample Rate Generator Register 1
#define MCBSP2_MCR2  (*(volatile ioport int*)0x300C) //  Multichannel Register 2
#define MCBSP2_MCR1  (*(volatile ioport int*)0x300D) //  Multichannel Register 1
#define MCBSP2_RCERA (*(volatile ioport int*)0x300E) //  Receive Channel Enable Register Partition A
#define MCBSP2_RCERB (*(volatile ioport int*)0x300F) //  Receive Channel Enable Register Partition B
#define MCBSP2_XCERA (*(volatile ioport int*)0x3010) //  Transmit Channel Enable Register Partition A
#define MCBSP2_XCERB (*(volatile ioport int*)0x3011) //  Transmit Channel Enable Register Partition B
#define MCBSP2_PCR   (*(volatile ioport int*)0x3012) //  Pin Control Register
#define MCBSP2_RCERC (*(volatile ioport int*)0x3013) //  Receive Channel Enable Register Partition C
#define MCBSP2_RCERD (*(volatile ioport int*)0x3014) //  Receive Channel Enable Register Partition D
#define MCBSP2_XCERC (*(volatile ioport int*)0x3015) //  Transmit Channel Enable Register Partition C
#define MCBSP2_XCERD (*(volatile ioport int*)0x3016) //  Transmit Channel Enable Register Partition D
#define MCBSP2_RCERE (*(volatile ioport int*)0x3017) //  Receive Channel Enable Register Partition E
#define MCBSP2_RCERF (*(volatile ioport int*)0x3018) //  Receive Channel Enable Register Partition F
#define MCBSP2_XCERE (*(volatile ioport int*)0x3019) //  Transmit Channel Enable Register Partition E
#define MCBSP2_XCERF (*(volatile ioport int*)0x301A) //  Transmit Channel Enable Register Partition F
#define MCBSP2_RCERG (*(volatile ioport int*)0x301B) //  Receive Channel Enable Register Partition G
#define MCBSP2_RCERH (*(volatile ioport int*)0x301C) //  Receive Channel Enable Register Partition H
#define MCBSP2_XCERG (*(volatile ioport int*)0x301D) //  Transmit Channel Enable Register Partition G
#define MCBSP2_XCERH (*(volatile ioport int*)0x301E) //  Transmit Channel Enable Register Partition H
//------------------------------------------------------------------------------
// SPCR1 register
//------------------------------------------------------------------------------
#define SPCR1_DLB                  0x8000
#define SPCR1_RJUST_RIGHT_ZERRO    0x0000
#define SPCR1_RJUST_RIGHT_SIGN     0x2000
#define SPCR1_RJUST_LEFT_ZERRO     0x4000
#define SPCR1_RJUST_LEFT_SIGN      0x6000
#define SPCR1_CLKSTP_DISABLE       0x0000
#define SPCR1_CLKSTP_WO_CLK_DLY    0x1000
#define SPCR1_CLKSTP_HALF_CLK_DLY  0x1800
#define SPCR1_DXENA                0x0080
#define SPCR1_ABIS                 0x0040
#define SPCR1_RINTM_RRDY           0x0000
#define SPCR1_RINTM_16_CHAN_BLOCK  0x0010
#define SPCR1_RINTM_FRAME_SYNC     0x0020
#define SPCR1_RINTM_RSYNCERR       0x0030
#define SPCR1_RSYNCERR             0x0008
#define SPCR1_RFULL                0x0004
#define SPCR1_RRDY                 0x0002
#define SPCR1_RRST_                0x0001
// bits
#define _SPCR1_RRDY                1
//-------------------------------------------------------------------------------
// SPCR2 register
//-------------------------------------------------------------------------------
#define SPCR2_FREE                 0x0200
#define SPCR2_SOFT                 0x0100
#define SPCR2_FRST_                0x0080
#define SPCR2_GRST_                0x0040
#define SPCR2_XINTM_XRDY           0x0000
#define SPCR2_XINTM_16_CHAN_BLOCK  0x0010
#define SPCR2_XINTM_FRAME_SYNC     0x0020
#define SPCR2_XINTM_XSYNCERR       0x0030
#define SPCR2_XSYNCERR             0x0008
#define SPCR2_XEMPTY_              0x0004
#define SPCR2_XRDY                 0x0002
#define SPCR2_XRST_                0x0001
// bits
#define _SPCR2_XRDY                1
//-------------------------------------------------------------------------------
// RCR1
//-------------------------------------------------------------------------------
#define RCR1_RFRLEN1(v)           ((v)<<8)
#define RCR1_RWDLEN1_8            0x0000
#define RCR1_RWDLEN1_12           0x0020
#define RCR1_RWDLEN1_16           0x0040
#define RCR1_RWDLEN1_20           0x0060
#define RCR1_RWDLEN1_24           0x0080
#define RCR1_RWDLEN1_32           0x00A0
//------------------------------------------------------------------------------
// RCR2
//------------------------------------------------------------------------------
#define RCR2_RPHASE                0x8000
#define RCR2_RFRLEN2(v)            ((v)<<8)
#define RCR2_RWDLEN2_8             0x0000
#define RCR2_RWDLEN2_12            0x0020
#define RCR2_RWDLEN2_16            0x0040
#define RCR2_RWDLEN2_20            0x0060
#define RCR2_RWDLEN2_24            0x0080
#define RCR2_RWDLEN2_32            0x00A0
#define RCR2_RCOMPAND_NO_COMP_MSB  0x0000
#define RCR2_RCOMPAND_NO_COMP_LSB  0x0008
#define RCR2_RCOMPAND_MU_LAW       0x0010
#define RCR2_RCOMPAND_A_LAW        0x0018
#define RCR2_RFIG                  0x0004
#define RCR2_RDATDLY_0_BIT         0x0000
#define RCR2_RDATDLY_1_BIT         0x0001
#define RCR2_RDATDLY_2_BIT         0x0002
//------------------------------------------------------------------------------
// XCR1
//------------------------------------------------------------------------------
#define XCR1_XFRLEN1(v)            ((v)<<8)
#define XCR1_XWDLEN1_8             0x0000
#define XCR1_XWDLEN1_12            0x0020
#define XCR1_XWDLEN1_16            0x0040
#define XCR1_XWDLEN1_20            0x0060
#define XCR1_XWDLEN1_24            0x0080
#define XCR1_XWDLEN1_32            0x00A0
//------------------------------------------------------------------------------
// XCR2
//------------------------------------------------------------------------------
#define XCR2_XPHASE                0x8000
#define XCR2_XFRLEN2(v)           ((v)<<8)
#define XCR2_XWDLEN2_8             0x0000
#define XCR2_XWDLEN2_12            0x0020
#define XCR2_XWDLEN2_16            0x0040
#define XCR2_XWDLEN2_20            0x0060
#define XCR2_XWDLEN2_24            0x0080
#define XCR2_XWDLEN2_32            0x00A0
#define XCR2_XCOMPAND_NO_COMP_MSB  0x0000
#define XCR2_XCOMPAND_NO_COMP_LSB  0x0008
#define XCR2_XCOMPAND_MU_LAW       0x0010
#define XCR2_XCOMPAND_A_LAW        0x0018
#define XCR2_XFIG                  0x0004
#define XCR2_XDATDLY_0_BIT         0x0000
#define XCR2_XDATDLY_1_BIT         0x0001
#define XCR2_XDATDLY_2_BIT         0x0002
//------------------------------------------------------------------------------
// SRGR1
//------------------------------------------------------------------------------
#define SRGR1_FWID(v)     ((v)<<8)
#define SRGR1_CLKGDV(v)   (v)
//------------------------------------------------------------------------------
// SRGR2
//------------------------------------------------------------------------------
#define SRGR2_GSYNC                0x8000
#define SRGR2_CLKSP                0x4000
#define SRGR2_CLKSM                0x2000
#define SRGR2_FSGM                 0x1000
#define SRGR2_FPER(v)              (v)
//------------------------------------------------------------------------------
// MCR1
//------------------------------------------------------------------------------
#define MCR1_RMCME                 0x0200
#define MCR1_RPBBLK_1              0x0000
#define MCR1_RPBBLK_3              0x0080
#define MCR1_RPBBLK_5              0x0100
#define MCR1_RPBBLK_7              0x0180
#define MCR1_RPABLK_0              0x0000
#define MCR1_RPABLK_2              0x0020
#define MCR1_RPABLK_4              0x0040
#define MCR1_RPABLK_6              0x0060
#define MCR1_RCBLK_0               0x0000
#define MCR1_RCBLK_1               0x0004
#define MCR1_RCBLK_2               0x0008
#define MCR1_RCBLK_3               0x000C
#define MCR1_RCBLK_4               0x0010
#define MCR1_RCBLK_5               0x0014
#define MCR1_RCBLK_6               0x0018
#define MCR1_RCBLK_7               0x001C
#define MCR1_RMCM                  0x0001
//-------------------------------------------------------------------------------
// MCR2
//-------------------------------------------------------------------------------
#define MCR2_XMCME                 0x0200
#define MCR2_XPBBLK_1              0x0000
#define MCR2_XPBBLK_3              0x0080
#define MCR2_XPBBLK_5              0x0100
#define MCR2_XPBBLK_7              0x0180
#define MCR2_XPABLK_0              0x0000
#define MCR2_XPABLK_2              0x0020
#define MCR2_XPABLK_4              0x0040
#define MCR2_XPABLK_6              0x0060
#define MCR2_XCBLK_0               0x0000
#define MCR2_XCBLK_1               0x0004
#define MCR2_XCBLK_2               0x0008
#define MCR2_XCBLK_3               0x000C
#define MCR2_XCBLK_4               0x0010
#define MCR2_XCBLK_5               0x0014
#define MCR2_XCBLK_6               0x0018
#define MCR2_XCBLK_7               0x001C
// TODO XMCM bits 1-0
#define MCR2_XMCM                  0x0001
//------------------------------------------------------------------------------
// PCR
//------------------------------------------------------------------------------
#define PCR_IDLE_EN                0x4000
#define PCR_XIOEN                  0x2000
#define PCR_RIOEN                  0x1000
#define PCR_FSXM                   0x0800
#define PCR_FSRM                   0x0400
#define PCR_CLKXM                  0x0200
#define PCR_CLKRM                  0x0100
#define PCR_SCLKME                 0x0080
#define PCR_CLKS_STAT              0x0040
#define PCR_DX_STAT                0x0020
#define PCR_DR_STAT                0x0010
#define PCR_FSXP                   0x0008
#define PCR_FSRP                   0x0004
#define PCR_CLKXP                  0x0002
#define PCR_CLKRP                  0x0001
/*******************************************************************************
 DMA module
 *******************************************************************************/
#define CSDP  	0x00
#define CCR   	0x01
#define CICR  	0x02
#define CSR   	0x03
#define CSSA_L	0x04
#define CSSA_U 	0x05
#define CDSA_L  0x06
#define CDSA_U  0x07
#define CEN  	0x08
#define CFN  	0x09
#define CFI 	0x0A
#define CEI 	0x0B
//
#define DMA0(r)    (*(volatile ioport int*)(0x0C00 + (r)))
#define DMA1(r)    (*(volatile ioport int*)(0x0C20 + (r)))
#define DMA2(r)    (*(volatile ioport int*)(0x0C40 + (r)))
#define DMA3(r)    (*(volatile ioport int*)(0x0C60 + (r)))
#define DMA4(r)    (*(volatile ioport int*)(0x0C80 + (r)))
#define DMA5(r)    (*(volatile ioport int*)(0x0CA0 + (r)))

#define DMA_GCR  	(*(volatile ioport int*)(0x0E00))
#define DMA_GTCR  	(*(volatile ioport int*)(0x0E03))
//-------------------------------------------------------------------------------
#define ENABLE_DMA0_INT()             asm("     OR #0x0004, *(IER1)");
#define DISABLE_DMA0_INT()            asm("     AND #0xFFFB, *(IER1)");
#define CLEAR_DMA0_INT()              asm("     OR #0x0004, *(IFR1)");
//-------------------------------------------------------------------------------
#define ENABLE_DMA1_INT()             asm("     OR #0x0200, *(IER0)");
#define DISABLE_DMA1_INT()            asm("     AND #0xFDFF, *(IER0)");
#define CLEAR_DMA1_INT()              asm("     OR #0x0200, *(IFR0)");
//-------------------------------------------------------------------------------
#define ENABLE_DMA2_INT()             asm("     OR #0x0010, *(IER1)");
#define DISABLE_DMA2_INT()            asm("     AND #0xFFEF, *(IER1)");
#define CLEAR_DMA2_INT()              asm("     OR #0x0010, *(IFR1)");
//-------------------------------------------------------------------------------
#define ENABLE_DMA3_INT()             asm("     OR #0x0020, *(IER1)");
#define DISABLE_DMA3_INT()            asm("     AND #0xFFDF, *(IER1)");
#define CLEAR_DMA3_INT()              asm("     OR #0x0020, *(IFR1)");
//-------------------------------------------------------------------------------
#define ENABLE_DMA4_INT()             asm("     OR #0x4000, *(IER0)");
#define DISABLE_DMA4_INT()            asm("     AND #0xBFFF, *(IER0)");
#define CLEAR_DMA4_INT()              asm("     OR #0x4000, *(IFR0)");
//-------------------------------------------------------------------------------
#define ENABLE_DMA5_INT()             asm("     OR #0x8000, *(IER0)");
#define DISABLE_DMA5_INT()            asm("     AND #0x7FFF, *(IER0)");
#define CLEAR_DMA5_INT()              asm("     OR #0x8000, *(IFR0)");
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
// GCR
//-------------------------------------------------------------------------------
#define GCR_FREE        0x0004
#define GCR_EHPI_EXCL   0x0002
#define GCR_EHPI_PRIO   0x0001
//-------------------------------------------------------------------------------
// CCR
//-------------------------------------------------------------------------------
#define CCR_DST_AMODE_CONST        0x0000
#define CCR_DST_AMODE_AUTO_INC     0x4000
#define CCR_DST_AMODE_SINGLE_IDX   0x8000
#define CCR_DST_AMODE_DOUBLE_IDX   0xC000
#define CCR_SRC_AMODE_CONST        0x0000
#define CCR_SRC_AMODE_AUTO_INC     0x1000
#define CCR_SRC_AMODE_SINGLE_IDX   0x2000
#define CCR_SRC_AMODE_DOUBLE_IDX   0x3000
#define CCR_END_PROG               0x0800
#define CCR_FIFO_FLUSH             0x0400
#define CCR_REPEAT                 0x0200
#define CCR_AUTOINIT               0x0100
#define CCR_EN                     0x0080
#define CCR_PRIO                   0x0040
#define CCR_FS                     0x0020
#define CCR_SYNC_NO                0x0000
#define CCR_SYNC_MCBSP0_RX         0x0001
#define CCR_SYNC_MCBSP0_TX         0x0002
#define CCR_SYNC_MCBSP1_RX         0x0005
#define CCR_SYNC_MCBSP1_TX         0x0006
#define CCR_SYNC_MCBSP2_RX         0x0009
#define CCR_SYNC_MCBSP2_TX         0x000A
#define CCR_SYNC_TIMER1            0x000D
#define CCR_SYNC_TIMER2            0x000E
#define CCR_SYNC_EXT_INT0          0x000F
#define CCR_SYNC_EXT_INT1          0x0010
#define CCR_SYNC_EXT_INT2          0x0011
#define CCR_SYNC_EXT_INT3          0x0012
#define CCR_SYNC_I2C_RX            0x0013
#define CCR_SYNC_I2C_TX            0x0014
//-------------------------------------------------------------------------------
// CSDP
//-------------------------------------------------------------------------------
#define CSDP_DST_BEN               
#define CSDP_DST_PACK              
#define CSDP_DST_SARAM             0x0000
#define CSDP_DST_DARAM             0x0200
#define CSDP_DST_EMIF              0x0400
#define CSDP_DST_IOPORT            0x0600
#define CSDP_SRC_BEN               
#define CSDP_SRC_PACK              
#define CSDP_SRC_SARAM             0x0000
#define CSDP_SRC_DARAM             0x0004
#define CSDP_SRC_EMIF              0x0008
#define CSDP_SRC_IOPORT            0x000C
#define CSDP_DATA_TYPE_8           0x0000
#define CSDP_DATA_TYPE_16          0x0001
#define CSDP_DATA_TYPE_32          0x0002
//-------------------------------------------------------------------------------
// CICR
//-------------------------------------------------------------------------------
#define CICR_BLOCK_IE              0x0020
#define CICR_LAST_IE               0x0010
#define CICR_FRAME_IE              0x0008
#define CICR_HALF_IE               0x0004
#define CICR_DROP_IE               0x0002
#define CICR_TIMEOUT               0x0001
// bits
#define _CICR_BLOCK_IE              5
#define _CICR_LAST_IE               4
#define _CICR_FRAME_IE              3
#define _CICR_HALF_IE               2
#define _CICR_DROP_IE               1
#define _CICR_TIMEOUT               0
//-------------------------------------------------------------------------------
// CSR
//-------------------------------------------------------------------------------
// bits
#define _CSR_BLOCK_IE               5
#define _CSR_LAST_IE                4
#define _CSR_FRAME_IE               3
#define _CSR_HALF_IE                2
#define _CSR_DROP_IE                1
#define _CSR_TIMEOUT                0

/*******************************************************************************
 * Watchdog Timer Registers                                                    *
 *******************************************************************************/
#define WDTIM            (*(volatile ioport int*)0x4000)   // WD Timer Counter Register
#define WDPRD            (*(volatile ioport int*)0x4001)   // WD Timer Period Register
#define WDTCR            (*(volatile ioport int*)0x4002)   // WD Timer Control Register
#define WDTCR2           (*(volatile ioport int*)0x4003)   // WD Timer Control Register 2

#define WDKEY1           0x05C6
#define WDKEY2           0x0A7E
/*******************************************************************************
 Vectors
 *******************************************************************************/
#define RESET_VECNO      0       // Reset (hardware and software)
#define NMI_VECNO        1       // Nonmaskable interrupt
#define INT0_VECNO       2       // External interrupt #0
#define INT2_VECNO       3       // External interrupt #2
#define TINT0_VECNO      4       // Timer #0 interrupt
#define RINT0_VECNO      5       // McBSP #0 receive interrupt
#define RINT1_VECNO      6       // McBSP #1 receive interrupt
#define XINT1_VECNO      7       // McBSP #1 transmit interrupt
#define MMCSD1_VECNO     7       // MMC/SD #1 interrupt
#define USB_VECNO        8       // USB interrupt
#define DMAC1_VECNO      9       // DMA Channel #1 interrupt
#define DSPINT_VECNO    10       // Interrupt from host
#define INT3_VECNO      11       // External interrupt #3
#define WDTINT_VECNO    11       // Watchdog timer interrupt
#define RINT2_VECNO     12       // McBSP #2 receive interrupt
#define XINT2_VECNO     13       // McBSP #2 transmit interrupt
#define MMCSD2_VECNO    13       // MMC/SD #2 interrupt
#define DMAC4_VECNO     14       // DMA Channel #4 interrupt
#define DMAC5_VECNO     15       // DMA Channel #5 interrupt
#define INT1_VECNO      16       // External interrupt #1
#define XINT0_VECNO     17       // McBSP #0 transmit interrupt
#define DMAC0_VECNO     18       // DMA Channel #0 interrupt
#define INT4_VECNO      19       // External interrupt #4
#define RTC_VECNO       19       // RTC interrupt
#define DMAC2_VECNO     20       // DMA Channel #2 interrupt
#define DMAC3_VECNO     21       // DMA Channel #3 interrupt
#define TINT1_VECNO     22       // Timer #1 interrupt
#define I2C_VECNO       23       // I2C interrupt
#define BERR_VECNO      24       // Bus Error interrupt
#define DLOG_VECNO      25       // Data Log interrupt
#define RTOS_VECNO      26       // Real-time Operating System interrupt
#define SINT27_VECNO    27       // Software interrupt #27
#define SINT28_VECNO    28       // Software interrupt #28
#define SINT29_VECNO    29       // Software interrupt #29
#define SINT30_VECNO    30       // Software interrupt #30
#define SINT31_VECNO    31       // Software interrupt #31

#endif // _TMS320C55_H_
