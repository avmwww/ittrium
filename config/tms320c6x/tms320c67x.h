#ifndef _TMS320C67X_H_
#define _TMS320C67X_H_


extern cregister volatile unsigned int ISTP; //Interrupt set register
extern cregister volatile unsigned int IRP;  //Interrupt return pointer
extern cregister volatile unsigned int NRP;  //Nonmaskable interrupt return pointer
extern cregister volatile unsigned int FADCR;//(C6700 only) Floating-point adder configuration register
extern cregister volatile unsigned int FAUCR;//(C6700 only) Floating-point auxiliary configuration register
extern cregister volatile unsigned int FMCR; //(C6700 only) Floating-point multiplier configuration register
extern cregister volatile unsigned int IFR;  //Interrupt flag register
extern cregister volatile unsigned int ISR;  //Interrupt set register
extern cregister volatile unsigned int ICR;  //Interrupt clear register
extern cregister volatile unsigned int IER;  //Interrupt enable register
extern cregister volatile unsigned int AMR;	 //Addressing mode register
extern cregister volatile unsigned int CSR;  //Control status register

#define _CSR_GIE		0
#define _CSR_PGIE		1
#define _CSR_DCC		2
#define _CSR_PCC 		5
#define _CSR_EN			8
#define _CSR_SAT		9
#define _CSR_PWRD 		10
#define _CSR_REVID		16
#define _CSR_CPUID		24

#define CSR_GIE         0x00000001
#define CSR_PGIE        0x00000002
#define CSR_DCC         0x00000004
#define CSR_PCC         0x00000020
#define CSR_EN          0x00000100
#define CSR_SAT         0x00000200
#define CSR_PWRD        0x00000400
#define CSR_REVID       0x00010000
#define CSR_CPUID       0x01000000

#define INTR_ENABLE(n)  IER |= 0x1<<(n)
#define INTR_DISABLE(n) IER &= ~(0x1<<(n))
#define INTR_CLEAR(n)   ICR = 0x1<<(n)

/*******************************************************************************
 Interrupt Selector Registers
 *******************************************************************************/
// Selects which interrupts drive CPU interrupts 10-15 (INT10-INT15)
#define MUXH            *(volatile unsigned int*)(0x019C0000)
#define _MUXH_INT_10    0
#define _MUXH_INT_11    5
#define _MUXH_INT_12    10
#define _MUXH_INT_13    16
#define _MUXH_INT_14    21
#define _MUXH_INT_15    26
// Selects which interrupts drive CPU interrupts 4-9 (INT04-INT09)
#define MUXL            *(volatile unsigned int*)(0x019C0004)
#define _MUXL_INT_04    0
#define _MUXL_INT_05    5
#define _MUXL_INT_06    10
#define _MUXL_INT_07    16
#define _MUXL_INT_08    21
#define _MUXL_INT_09    26
// Sets the polarity of the external interrupts (EXT_INT4-EXT_INT7)
#define EXTPOL          *(volatile unsigned int*)(0x019C0008)
//Interrupt event
#define	IEV_DSPINT      0x00 //HPI
#define	IEV_TINT0       0x01 //Timer 0
#define	IEV_TINT1       0x02 //Timer 1
#define	IEV_SDINT       0x03 //EMIF 
#define	IEV_GPINT4      0x04 //GPIO   
#define	IEV_GPINT5      0x05 //GPIO   
#define	IEV_GPINT6      0x06 //GPIO   
#define	IEV_GPINT7      0x07 //GPIO   
#define	IEV_EDMAINT     0x08 //EDMA   
#define	IEV_EMUDTDMA    0x09 //Emulation    
#define	IEV_EMURTDXRX   0x0A //Emulation     
#define	IEV_EMURTDXTX   0x0B //Emulation     
#define	IEV_XINT0       0x0C //McBSP0 
#define	IEV_RINT0       0x0D //McBSP0 
#define	IEV_XINT1       0x0E //McBSP1 
#define	IEV_RINT1       0x0F //McBSP1 
#define	IEV_GPINT0      0x10 //GPIO  
#define	IEV_I2CINT0     0x16 //I2C0   
#define	IEV_I2CINT1     0x17 //I2C1   
#define	IEV_AXINT0      0x1C //McASP0  
#define	IEV_ARINT0      0x1D //McASP0  
#define	IEV_AXINT1      0x1E //McASP1  
#define	IEV_ARINT1      0x1F //McASP1  
// Interrupt number
#define RESET_INT_NO            0x00
#define NMI_INT_NO              0x01
#define GPIO4_INT_NO            0x04
#define GPIO5_INT_NO            0x05 
#define GPIO6_INT_NO            0x06 
#define GPIO7_INT_NO            0x07 
#define EDMA_INT_NO             0x08 
#define EMUDTDMA_INT_NO         0x09 
#define SD_INT_NO               0x10 
#define EMURTDXRX_INT_NO        0x11
#define EMURTDXTX_INT_NO        0x12 
#define DSP_INT_NO              0x13 
#define TIMER0_INT_NO           0x14 
#define TIMER1_INT_NO           0x15 


#define ENABLE_EDMA_INT()       INTR_ENABLE(8)
//#define ENABLE_I2C_INT		INTR_ENB(7)
//#define ENABLE_TINT0_INT	INTR_ENB(14)

/*******************************************************************************
 Timers Registers
 *******************************************************************************/
#define CTL_0           *(volatile unsigned int*)(0x01940000)
#define CTL_1           *(volatile unsigned int*)(0x01980000)
#define CTL(n)          *(volatile unsigned int*)(0x01940000 + n*0x00040000)

#define _CTL_FUNC			0
#define _CTL_INVOUT			1
#define _CTL_DATOUT			2
#define _CTL_DATIN			3
#define _CTL_PWID			4
#define _CTL_GO				6
#define _CTL_HLD			7
#define _CTL_C_P			8
#define _CTL_CLKSRC			9
#define _CTL_INVINP			10
#define _CTL_TSTAT			11

#define CTL_FUNC        0x0001
#define CTL_INVOUT      0x0002
#define CTL_DATOUT      0x0004
#define CTL_DATIN       0x0008
#define CTL_PWID(a)     ((a)<<4)
#define CTL_GO          0x0040
#define CTL_HLD         0x0080
#define CTL_C_P         0x0100
#define CTL_CLKSRC      0x0200
#define CTL_INVINP      0x0400
#define CTL_TSTAT       0x0800

#define START_TIMER_0   CTL_0 |= CTL_HLD
#define START_TIMER_1   CTL_1 |= CTL_HLD
#define START_TIMER(n)  CTL(n) |= CTL_HLD

#define STOP_TIMER_0    CTL_10&= ~CTL_HLD
#define STOP_TIMER_1    CTL_1 &= ~CTL_HLD
#define STOP_TIMER(n)   CTL(n) &= ~CTL_HLD

#define PRD_0           *(volatile unsigned int*)(0x01940004)
#define PRD_1           *(volatile unsigned int*)(0x01980004)
#define PRD(n)	        *(volatile unsigned int*)(0x01940004 + n*0x00040000)

#define CNT_0           *(volatile unsigned int*)(0x01940008)
#define CNT_1           *(volatile unsigned int*)(0x01980008)
#define CNT(n)          *(volatile unsigned int*)(0x01940008 + n*0x00040000)

#define ENABLE_TINT0_INT()      INTR_ENABLE(14)
#define DISABLE_TINT0_INT()     INTR_DISABLE(14)

/*******************************************************************************
 PLL Controller Registers 
 *******************************************************************************/
#define PLLCSR          *(volatile unsigned int*)(0x01b7c100)

#define PLLCSR_PLLEN    0x01
#define PLLCSR_PLLPWRDN 0x02
#define PLLCSR_PLLRST   0x08
#define PLLCSR_STABLE   0x40

#define PLLM            *(volatile unsigned int*)(0x01b7c110)
#define PLLDIV0         *(volatile unsigned int*)(0x01b7c114)
#define PLLDIV1         *(volatile unsigned int*)(0x01b7c118)
#define PLLDIV2         *(volatile unsigned int*)(0x01b7c11c)
#define PLLDIV3         *(volatile unsigned int*)(0x01b7c120)
#define OSCDIV1         *(volatile unsigned int*)(0x01b7c124)

#define OSCDIV1_OD1EN   0x00008000
#define PLLDIV_D_EN     0x00008000
/*******************************************************************************
 Device Configuration
 *******************************************************************************/
#define DEVCFG          *(volatile unsigned int*)0x019C0200

/*******************************************************************************
 GPIO Registers
 *******************************************************************************/
#define GPEN            *(volatile unsigned int*)(0x01b00000)
#define GPDIR           *(volatile unsigned int*)(0x01b00004)
#define GPVAL           *(volatile unsigned int*)(0x01b00008)

/*******************************************************************************
 EMIF Registers
 *******************************************************************************/
#define GBLCTL          *(volatile unsigned int*)(0x01800000)

#define _GBLCTL_CLK2EN  3
#define _GBLCTL_CLK1EN  4
#define _GBLCTL_EKEN    5
#define _GBLCTL_NOHOLD  7
#define _GBLCTL_HOLDA   8
#define _GBLCTL_HOLD    9
#define _GBLCTL_ARDY    10
#define _GBLCTL_BUSREQ  11

#define GBLCTL_CLK2EN   0x00000008
#define GBLCTL_CLK1EN   0x00000010
#define GBLCTL_EKEN     0x00000020
#define GBLCTL_NOHOLD   0x00000080
#define GBLCTL_HOLDA    0x00000100
#define GBLCTL_HOLD     0x00000200
#define GBLCTL_ARDY     0x00000400
#define GBLCTL_BUSREQ   0x00000800

#define CECTL1          *(volatile unsigned int*)(0x01800004)
#define CECTL0          *(volatile unsigned int*)(0x01800008)
#define CECTL2          *(volatile unsigned int*)(0x01800010)
#define CECTL3          *(volatile unsigned int*)(0x01800014)

#define CECTL_MTYPE_ASYNC8      0x00000000
#define CECTL_MTYPE_ASYNC16     0x00000010
#define CECTL_MTYPE_ASYNC32     0x00000020
#define CECTL_MTYPE_SDRAM32     0x00000030
#define CECTL_MTYPE_SBSRAM32    0x00000040
#define CECTL_MTYPE_SDRAM8      0x00000080
#define CECTL_MTYPE_SDRAM16     0x00000090
#define CECTL_MTYPE_SBSRAM8     0x000000A0
#define CECTL_MTYPE_SBSRAM16    0x000000B0

#define CECTL_RDHLD(x)          (x)
#define CECTL_MTYPE(x)          ((x)<<4)
#define CECTL_RDSTRB(x)         ((x)<<8)
#define CECTL_TA(x)             ((x)<<14)
#define CECTL_RDSETUP(x)        ((x)<<16)
#define CECTL_WRHLD(x)          ((x)<<20)
#define CECTL_WRSTRB(x)         ((x)<<22)
#define CECTL_WRSETUP(x)        ((x)<<28)

#define _CECTL_RDHLD             0
#define _CECTL_MTYPE             4
#define _CECTL_RDSTRB            8
#define _CECTL_TA               14
#define _CECTL_RDSETUP          16
#define _CECTL_WRHLD            20
#define _CECTL_WRSTRB           22
#define _CECTL_WRSETUP          28

#define CE0_ADDR(addr)          (0x80000000 + (addr))
#define CE1_ADDR(addr)          (0x90000000 + (addr))
#define CE2_ADDR(addr)          (0xA0000000 + (addr))
#define CE3_ADDR(addr)          (0xB0000000 + (addr))

#define SDCTL                   *(volatile unsigned int*)(0x01800018)
#define _SDCTL_TRC              12
#define _SDCTL_TRP              16
#define _SDCTL_TRCD             20
#define _SDCTL_INIT             24
#define _SDCTL_RFEN             25
#define _SDCTL_SDCSZ            26
#define _SDCTL_SDRSZ            28
#define _SDCTL_SDBSZ            30

#define SDTIM                   *(volatile unsigned int*)0x0180001C
#define _SDTIM_PERIOD           0
#define _SDTIM_CNTR             12
#define _SDTIM_XRFR             24

#define SDEXT                   *(volatile unsigned int*)0x01800020

/*******************************************************************************
 I2C Registers
 *******************************************************************************/

//I2C0 Registers
#define I2COAR0         *(volatile unsigned int*)(0x01B40000)   //I2C0 own address register
#define I2CIER0         *(volatile unsigned int*)(0x01B40004)   //I2C0 interrupt enable register
#define I2CSTR0         *(volatile unsigned int*)(0x01B40008)   //I2C0 interrupt status register
#define I2CCLKL0        *(volatile unsigned int*)(0x01B4000C)   //I2C0 clock low-time divider register
#define I2CCLKH0        *(volatile unsigned int*)(0x01B40010)   //I2C0 clock high-time divider register
#define I2CCNT0         *(volatile unsigned int*)(0x01B40014)   //I2C0 data count register
#define I2CDRR0         *(volatile unsigned int*)(0x01B40018)   //I2C0 data receive register
#define I2CSAR0         *(volatile unsigned int*)(0x01B4001C)   //I2C0 slave address register
#define I2CDXR0         *(volatile unsigned int*)(0x01B40020)   //I2C0 data transmit register
#define I2CMDR0         *(volatile unsigned int*)(0x01B40024)   //I2C0 mode register
#define I2CISRC0        *(volatile unsigned int*)(0x01B40028)   //I2C0 interrupt source register
#define I2CPSC0         *(volatile unsigned int*)(0x01B40030)   //I2C0 prescaler register

//I2C ~ I2C0
#define I2COAR          I2COAR0
//I2C Interrupt Enable Register
#define I2CIER          I2CIER0
#define I2CIER_AL       0x01
#define I2CIER_NACK     0x02
#define I2CIER_ARDY     0x04
#define I2CIER_ICRRDY   0x08
#define I2CIER_RRDY     I2CIER_ICRRDY
#define I2CIER_ICXRDY   0x10
#define I2CIER_XRDY     I2CIER_ICXRDY
//I2C Status Register
#define I2CSTR          I2CSTR0
#define I2CSTR_AL       0x0001
#define I2CSTR_NACK     0x0002
#define I2CSTR_ARDY     0x0004
#define I2CSTR_ICRRDY   0x0008
#define I2CSTR_RRDY     I2CSTR_ICRRDY
#define I2CSTR_ICXRDY   0x0010
#define I2CSTR_XRDY     I2CSTR_ICXRDY
#define I2CSTR_AD0      0x0100
#define I2CSTR_AAS      0x0200
#define I2CSTR_XSMT     0x0400
#define I2CSTR_RSFULL   0x0800
#define I2CSTR_BB       0x1000
#define I2CSTR_NACKSNT  0x2000
//                
#define I2CCLKL         I2CCLKL0
#define I2CCLKH         I2CCLKH0
#define I2CCNT          I2CCNT0 
#define I2CDRR          I2CDRR0 
#define I2CSAR          I2CSAR0 
#define I2CDXR          I2CDXR0 
//I2C Mode Register
#define I2CMDR          I2CMDR0
#define _I2CMDR_BC      0x0000
#define I2CMDR_FDF      0x0008
#define I2CMDR_STB      0x0010
#define I2CMDR_IRS      0x0020
#define I2CMDR_DLB      0x0040
#define I2CMDR_RM       0x0080
#define I2CMDR_XA       0x0100
#define I2CMDR_TRX      0x0200
#define I2CMDR_MST      0x0400
#define I2CMDR_STP      0x0800
#define I2CMDR_STT      0x2000
#define I2CMDR_FREE     0x4000
#define I2CMDR_NACKMOD  0x8000
//I2C Interrupt Source Register
#define I2CISRC                 I2CISRC0
#define I2CISRC_INTCODE         0x07
#define	I2CISRC_INTCODE_ARDY    0x3
#define	I2CISRC_INTCODE_RRDY    0x4
#define	I2CISRC_INTCODE_XRDY    0x5	
//
#define I2CPSC          I2CPSC0 
//I2C1 Registers
#define I2COAR1         *(volatile unsigned int*)(0x01B44000) //I2C1 own address register
#define I2CIER1         *(volatile unsigned int*)(0x01B44004) //I2C1 interrupt enable register
#define I2CSTR1         *(volatile unsigned int*)(0x01B44008) //I2C1 interrupt status register
#define I2CCLKL1        *(volatile unsigned int*)(0x01B4400C) //I2C1 clock low-time divider register
#define I2CCLKH1        *(volatile unsigned int*)(0x01B44010) //I2C1 clock high-time divider register
#define I2CCNT1         *(volatile unsigned int*)(0x01B44014) //I2C1 data count register
#define I2CDRR1         *(volatile unsigned int*)(0x01B44018) //I2C1 data receive register
#define I2CSAR1         *(volatile unsigned int*)(0x01B4401C) //I2C1 slave address register
#define I2CDXR1         *(volatile unsigned int*)(0x01B44020) //I2C1 data transmit register
#define I2CMDR1         *(volatile unsigned int*)(0x01B44024) //I2C1 mode register
#define I2CISRC1        *(volatile unsigned int*)(0x01B44028) //I2C1 interrupt source register
#define I2CPSC1         *(volatile unsigned int*)(0x01B44030) //I2C1 prescaler register

/*******************************************************************************
 McBSP0 and McBSP1 Registers
 *******************************************************************************/
#define MCBSP(n)        ((n)*0x00040000+0x018C0000)
#define MCBSP0          (0x018C0000)
#define MCBSP1          (0x01900000)

#define MCBSP_DRR(n)    *(volatile unsigned int*)(MCBSP(n)+0x0000)
#define MCBSP_DXR(n)    *(volatile unsigned int*)(MCBSP(n)+0x0004)
#define MCBSP_SPCR(n)   *(volatile unsigned int*)(MCBSP(n)+0x0008)
#define MCBSP_RCR(n)    *(volatile unsigned int*)(MCBSP(n)+0x000C)
#define MCBSP_XCR(n)    *(volatile unsigned int*)(MCBSP(n)+0x0010)
#define MCBSP_SRGR(n)   *(volatile unsigned int*)(MCBSP(n)+0x0014)
#define MCBSP_MCR(n)    *(volatile unsigned int*)(MCBSP(n)+0x0018)
#define MCBSP_RCER(n)   *(volatile unsigned int*)(MCBSP(n)+0x001C)
#define MCBSP_XCER(n)   *(volatile unsigned int*)(MCBSP(n)+0x0020)
#define MCBSP_PCR(n)    *(volatile unsigned int*)(MCBSP(n)+0x0024)

#define MCBSP0_DRR      *(volatile unsigned int*)(MCBSP0+0x0000)
#define MCBSP1_DRR      *(volatile unsigned int*)(MCBSP1+0x0000)
#define MCBSP0_DXR      *(volatile unsigned int*)(MCBSP0+0x0004)
#define MCBSP1_DXR      *(volatile unsigned int*)(MCBSP1+0x0004)
#define MCBSP0_SPCR     *(volatile unsigned int*)(MCBSP0+0x0008)
#define MCBSP1_SPCR     *(volatile unsigned int*)(MCBSP1+0x0008)
#define MCBSP0_RCR      *(volatile unsigned int*)(MCBSP0+0x000C)
#define MCBSP1_RCR      *(volatile unsigned int*)(MCBSP1+0x000C)
#define MCBSP0_XCR      *(volatile unsigned int*)(MCBSP0+0x0010)
#define MCBSP1_XCR      *(volatile unsigned int*)(MCBSP1+0x0010)
#define MCBSP0_SRGR     *(volatile unsigned int*)(MCBSP0+0x0014)
#define MCBSP1_SRGR     *(volatile unsigned int*)(MCBSP1+0x0014)
#define MCBSP0_MCR      *(volatile unsigned int*)(MCBSP0+0x0018)
#define MCBSP1_MCR      *(volatile unsigned int*)(MCBSP1+0x0018)
#define MCBSP0_RCER     *(volatile unsigned int*)(MCBSP0+0x001C)
#define MCBSP1_RCER     *(volatile unsigned int*)(MCBSP1+0x001C)
#define MCBSP0_XCER     *(volatile unsigned int*)(MCBSP0+0x0020)
#define MCBSP1_XCER     *(volatile unsigned int*)(MCBSP1+0x0020)
#define MCBSP0_PCR      *(volatile unsigned int*)(MCBSP0+0x0024)
#define MCBSP1_PCR      *(volatile unsigned int*)(MCBSP1+0x0024)
//------------------------------------------------------------------------------
// SPCR
//------------------------------------------------------------------------------
#define SPCR_RRST       0x00000001
#define SPCR_RRDY       0x00000002
#define SPCR_RFULL      0x00000004
#define SPCR_RSYNCERR   0x00000008
#define SPCR_RINTM(x)   ((x)<<4)
#define SPCR_DXENA      0x00000080
#define SPCR_CLKSTP(x)  ((x)<<11)
#define SPCR_RJUST(x)   ((x)<<13)
#define SPCR_DLB        0x00008000
#define SPCR_XRST       0x00010000
#define SPCR_XRDY       0x00020000
#define SPCR_XEMPTY     0x00040000
#define SPCR_XSYNCERR   0x00080000
#define SPCR_XINTM(x)   ((x)<<20)
#define SPCR_GRST       0x00400000
#define SPCR_FRST       0x00800000
#define SPCR_SOFT       0x01000000
#define SPCR_FREE       0x02000000

#define _SPCR_RRST       0
#define _SPCR_RRDY       1
#define _SPCR_RFULL      2
#define _SPCR_RSYNCERR   3
#define _SPCR_RINTM      4
#define _SPCR_DXENA      7
#define _SPCR_CLKSTP    11
#define _SPCR_RJUST     13
#define _SPCR_DLB       15
#define _SPCR_XRST      16
#define _SPCR_XRDY      17
#define _SPCR_XEMPTY    18
#define _SPCR_XSYNCERR  19
#define _SPCR_XINTM     20
#define _SPCR_GRST      22
#define _SPCR_FRST      23
#define _SPCR_SOFT      24
#define _SPCR_FREE      25
//------------------------------------------------------------------------------
// RCR
//------------------------------------------------------------------------------
#define RXCR_PHASE_SINGLE       0x00000000
#define RXCR_PHASE_DUAL         0x80000000
#define RXCR_PHASE(x)           ((x)<<31)
#define RXCR_FRLEN2(x)          ((x)<<24)
#define RXCR_WDLEN2_8           0x00000000
#define RXCR_WDLEN2_12          0x00200000
#define RXCR_WDLEN2_16          0x00400000
#define RXCR_WDLEN2_20          0x00600000
#define RXCR_WDLEN2_24          0x00800000
#define RXCR_WDLEN2_32          0x00A00000
#define RXCR_WDLEN2(x)          ((x)<<21)
#define RXCR_RCOMPAND_MSB       0x00000000
#define RXCR_RCOMPAND_8BITLSB   0x00080000
#define RXCR_RCOMPAND_ULAW      0x00100000
#define RXCR_RCOMPAND_ALAW      0x00180000
#define RXCR_RCOMPAND(x)        ((x)<<19)
#define RXCR_RFIG               0x00040000
#define RXCR_DATDLY_0BIT        0x00000000
#define RXCR_DATDLY_1BIT        0x00010000
#define RXCR_DATDLY_2BIT        0x00020000
#define RXCR_DATDLY(x)          ((x)<<16)
#define RXCR_PHASE2_NORMAL      0x00000000
#define RXCR_PHASE2_OPPOSITE    0x00008000
#define RXCR_FRLEN1(x)          ((x)<<8)
#define RXCR_WDLEN1_8           0x00000000
#define RXCR_WDLEN1_12          0x00000020
#define RXCR_WDLEN1_16          0x00000040
#define RXCR_WDLEN1_20          0x00000060
#define RXCR_WDLEN1_24          0x00000080
#define RXCR_WDLEN1_32          0x000000A0
#define RXCR_WDLEN1(x)          ((x)<<5)
#define RXCR_WDREVRS            0x00000010

#define _RXCR_WDREVRS            4
#define _RXCR_WDLEN1             5
#define _RXCR_FRLEN1             8
#define _RXCR_DATDLY            16
#define _RXCR_RFIG              18
#define _RXCR_RCOMPAND          19
#define _RXCR_WDLEN2            21
#define _RXCR_FRLEN2            24
#define _RXCR_PHASE             31
//------------------------------------------------------------------------------
// SRGR
//------------------------------------------------------------------------------
#define SRGR_CLKGDV(x)          (x)
#define SRGR_FWID(x)            ((x)<<8)
#define SRGR_FPER(x)            ((x)<<16)
#define SRGR_FSGM               0x10000000
#define SRGR_CLKSM              0x20000000
#define SRGR_CLKSP              0x40000000
#define SRGR_GSYNC              0x80000000

#define _SRGR_CLKGDV             0
#define _SRGR_FWID               8
#define _SRGR_FPER              16
#define _SRGR_FSGM              28
#define _SRGR_CLKSM             29
#define _SRGR_CLKSP             30
#define _SRGR_GSYNC             31
//------------------------------------------------------------------------------
// MCR
//------------------------------------------------------------------------------
#define MCR_RMCM                0x00000001
#define MCR_RCBLK(x)            ((x)<<2)
#define MCR_RPABLK(x)           ((x)<<5)
#define MCR_RPBBLK(x)           ((x)<<7)
#define MCR_RMCME               0x00002000
#define MCR_XMCM                0x00010000
#define MCR_XCBLK(x)            ((x)<<18)
#define MCR_XPABLK(x)           ((x)<<21)
#define MCR_XPBBLK(x)           ((x)<<23)
#define MCR_XMCME               0x02000000

#define _MCR_RMCM                0
#define _MCR_RCBLK               2
#define _MCR_RPABLK              5
#define _MCR_RPBBLK              7
#define _MCR_RMCME               9
#define _MCR_XMCM               16
#define _MCR_XCBLK              18
#define _MCR_XPABLK             21
#define _MCR_XPBBLK             23
//------------------------------------------------------------------------------
// PCR
//------------------------------------------------------------------------------
#define PCR_CLKRP               0x00000001
#define PCR_CLKXP               0x00000002
#define PCR_FSRP                0x00000004
#define PCR_FSXP                0x00000008
#define PCR_DRSTAT              0x00000010
#define PCR_DXSTAT              0x00000020
#define PCR_CLKSSTAT            0x00000040
#define PCR_CLKRM               0x00000100
#define PCR_CLKXM               0x00000200
#define PCR_FSRM                0x00000400
#define PCR_FSXM                0x00000800
#define PCR_RIOEN               0x00001000
#define PCR_XIOEN               0x00002000

#define _PCR_CLKRP               0
#define _PCR_CLKXP               1
#define _PCR_FSRP                2
#define _PCR_FSXP                3
#define _PCR_DR_STAT             4
#define _PCR_DX_STAT             5
#define _PCR_CLKS_STAT           6
#define _PCR_CLKRM               8
#define _PCR_CLKXM               9
#define _PCR_FSRM               10
#define _PCR_FSXM               11
#define _PCR_RIOEN              12
#define _PCR_XIOEN              13
/*******************************************************************************
 McASP0 and McASP1 Registers
 *******************************************************************************/
#define MCASP(n)                ((n)*0x00004000+0x01B4C000)
#define MCASP0                  (0x01B4C000)
#define MCASP1                  (0x01B50000)
//
#define MCASP0_MCASPPID         *(volatile unsigned int*)(MCASP0)
#define MCASP1_MCASPPID         *(volatile unsigned int*)(MCASP1)

#define MCASP_PWRDEMU(n)        *(volatile unsigned int*)(MCASP(n)+0x0004)
#define MCASP0_PWRDEMU          *(volatile unsigned int*)(MCASP0+0x0004)
#define MCASP1_PWRDEMU          *(volatile unsigned int*)(MCASP1+0x0004)

// Pin
#define PIN_AXR_0             0x00000001
#define PIN_AXR_1             0x00000002
#define PIN_AXR_2             0x00000004
#define PIN_AXR_3             0x00000008
#define PIN_AXR_4             0x00000010
#define PIN_AXR_5             0x00000020
#define PIN_AXR_6             0x00000040
#define PIN_AXR_7             0x00000080
#define PIN_AXR_8             0x00000100
#define PIN_AXR_9             0x00000200
#define PIN_AXR_10            0x00000400
#define PIN_AXR_11            0x00000800
#define PIN_AXR_12            0x00001000
#define PIN_AXR_13            0x00002000
#define PIN_AXR_14            0x00004000
#define PIN_AXR_15            0x00008000
#define PIN_AMUTE             0x02000000
#define PIN_ACLKX             0x04000000
#define PIN_AHCLKX            0x08000000
#define PIN_AFSX              0x10000000
#define PIN_ACLKR             0x20000000
#define PIN_AHCLKR            0x40000000
#define PIN_AFSR              0x80000000

#define MCASPGPIO_AXR_0         0x00000001
#define MCASPGPIO_AXR_1         0x00000002
#define MCASPGPIO_AXR_2         0x00000004
#define MCASPGPIO_AMUTE         0x02000000
#define MCASPGPIO_ACLKX         0x04000000
#define MCASPGPIO_AHCLKX        0x08000000
#define MCASPGPIO_AFSX          0x10000000
#define MCASPGPIO_ACLKR         0x20000000
#define MCASPGPIO_AHCLKR        0x40000000
#define MCASPGPIO_AFSR          0x80000000

#define _MCASPGPIO_AXR(n)       (n)
#define _MCASPGPIO_AMUTE        25
#define _MCASPGPIO_ACLKX        26
#define _MCASPGPIO_AHCLKX       27
#define _MCASPGPIO_AFSX         28
#define _MCASPGPIO_ACLKR        29
#define _MCASPGPIO_AHCLKR       30
#define _MCASPGPIO_AFSR         31

//Pin Function Register
#define MCASP_PFUNC(n)          *(volatile unsigned int*)(MCASP(n)+0x0010) 
//Pin Direction Register
#define MCASP_PDIR(n)           *(volatile unsigned int*)(MCASP(n)+0x0014) 
//Pin Data Output Register
#define MCASP_PDOUT(n)          *(volatile unsigned int*)(MCASP(n)+0x0018) 
//Pin Data Input Register
#define MCASP_PDIN(n)           *(volatile unsigned int*)(MCASP(n)+0x001C) 
//Pin Data Set Register
#define MCASP_PDSET(n)          *(volatile unsigned int*)(MCASP(n)+0x001C) 
//Pin Data Clear Register
#define MCASP_PDCLR(n)          *(volatile unsigned int*)(MCASP(n)+0x0020) 
//Global Control Register
#define MCASP_GBLCTL(n)         *(volatile unsigned int*)(MCASP(n)+0x0044)

#define _GBLCTL_RCLKRST          0
#define _GBLCTL_RHCLKRST         1
#define _GBLCTL_RSRCLR           2
#define _GBLCTL_RSMRST           3
#define _GBLCTL_RFRST            4
#define _GBLCTL_XCLKRST          8
#define _GBLCTL_XHCLKRST         9
#define _GBLCTL_XSRCLR          10
#define _GBLCTL_XSMRST          11
#define _GBLCTL_XFRST           12

#define GBLCTL_RCLKRST          0x00000001
#define GBLCTL_RHCLKRST         0x00000002
#define GBLCTL_RSRCLR           0x00000004
#define GBLCTL_RSMRST           0x00000008
#define GBLCTL_RFRST            0x00000010
#define GBLCTL_XCLKRST          0x00000100
#define GBLCTL_XHCLKRST         0x00000200
#define GBLCTL_XSRCLR           0x00000400
#define GBLCTL_XSMRST           0x00000800
#define GBLCTL_XFRST            0x00001000
//Audio Mute Control Register
#define MCASP_AMUTE(n)          *(volatile unsigned int*)(MCASP(n)+0x0048)

#define AMUTE_MUTEN             0
#define AMUTE_INPOL             2
#define AMUTE_INEN              3
#define AMUTE_INSTAT            4
#define AMUTE_ROVRN             5
#define AMUTE_XUNDRN            6
#define AMUTE_RSYNCERR          7
#define AMUTE_XSYNCERR          8
#define AMUTE_RCKFAIL           9
#define AMUTE_XCKFAIL          10
#define AMUTE_RDMAERR          11
#define AMUTE_XDMAERR          12

#define _AMUTE_MUTEN             0
#define _AMUTE_INPOL             2
#define _AMUTE_INEN              3
#define _AMUTE_INSTAT            4
#define _AMUTE_ROVRN             5
#define _AMUTE_XUNDRN            6
#define _AMUTE_RSYNCERR          7
#define _AMUTE_XSYNCERR          8
#define _AMUTE_RCKFAIL           9
#define _AMUTE_XCKFAIL          10
#define _AMUTE_RDMAERR          11
#define _AMUTE_XDMAERR          12
// Digital Loopback Control Register
#define MCASP_DLBCTL(n)         *(volatile unsigned int*)(MCASP(n)+0x004C)

#define DLBCTL_DLBEN            0x00000001
#define DLBCTL_ORD              0x00000002
#define DLBCTL_MODE             0x00000004

#define _DLBCTL_DLBEN           0
#define _DLBCTL_ORD             1
#define _DLBCTL_MODE            2

//DIT Mode Control Register
#define MCASP_DITCTL(n)         *(volatile unsigned int*)(MCASP(n)+0x0050)

#define DITCTL_DITEN            0x00000001
#define DITCTL_VA               0x00000004
#define DITCTL_VB               0x00000008

#define _DITCTL_DITEN            0
#define _DITCTL_VA               2
#define _DITCTL_VB               3

//Receiver and Transmitter Global Control Registers
#define MCASP_RGBLCTL(n)        *(volatile unsigned int*)(MCASP(n)+0x0060)
//Alias of GBLCTL containing only Receiver Reset bits,
//allows transmit to be reset independently from receive.
#define MCASP_XGBLCTL(n)        *(volatile unsigned int*)(MCASP(n)+0x00A0)
//Alias of GBLCTL containing only Transmitter Reset bits,
//allows transmit to be reset independently from receive.

//Receive and Transmit Format Unit Bit Mask Registers
#define MCASP_RMASK(n)          *(volatile unsigned int*)(MCASP(n)+0x0064)
#define MCASP_XMASK(n)          *(volatile unsigned int*)(MCASP(n)+0x00A4)

//Receive and Transmit Bit Stream Format Registers
#define MCASP_RFMT(n)           *(volatile unsigned int*)(MCASP(n)+0x0068)
#define MCASP_XFMT(n)           *(volatile unsigned int*)(MCASP(n)+0x00A8)

#define XFMT_ROT(x)             (x)
#define XFMT_ROT_0              0x00000000
#define XFMT_ROT_4              0x00000001
#define XFMT_ROT_8              0x00000002
#define XFMT_ROT_12             0x00000003
#define XFMT_ROT_16             0x00000004
#define XFMT_ROT_20             0x00000005
#define XFMT_ROT_24             0x00000006
#define XFMT_ROT_28             0x00000007
#define XFMT_BUSEL              0x00000008
#define XFMT_SSZ(x)             ((x)<<4)
#define XFMT_SSZ_8              0x00000030
#define XFMT_SSZ_12             0x00000050
#define XFMT_SSZ_16             0x00000070
#define XFMT_SSZ_20             0x00000090
#define XFMT_SSZ_24             0x000000B0
#define XFMT_SSZ_28             0x000000D0
#define XFMT_SSZ_32             0x000000F0
#define XFMT_PBIT(x)            ((x)<<8)
#define XFMT_PAD(x)             ((x)<<13)
#define XFMT_RVRS               0x00008000
#define XFMT_DATDLY(x)          ((x)<<16)

#define RFMT_ROT(x)             (x)
#define RFMT_ROT_0              0x00000000
#define RFMT_ROT_4              0x00000001
#define RFMT_ROT_8              0x00000002
#define RFMT_ROT_12             0x00000003
#define RFMT_ROT_16             0x00000004
#define RFMT_ROT_20             0x00000005
#define RFMT_ROT_24             0x00000006
#define RFMT_ROT_28             0x00000007
#define RFMT_BUSEL              0x00000008
#define RFMT_SSZ(x)             ((x)<<4)
#define RFMT_SSZ_8              0x00000030
#define RFMT_SSZ_12             0x00000050
#define RFMT_SSZ_16             0x00000070
#define RFMT_SSZ_20             0x00000090
#define RFMT_SSZ_24             0x000000B0
#define RFMT_SSZ_28             0x000000D0
#define RFMT_SSZ_32             0x000000F0
#define RFMT_PBIT(x)            ((x)<<8)
#define RFMT_PAD(x)             ((x)<<13)
#define RFMT_RVRS               0x00008000
#define RFMT_DATDLY(x)          ((x)<<16)

#define _XFMT_ROT                0
#define _XFMT_BUSEL              3
#define _XFMT_SSZ                4
#define _XFMT_PBIT               8
#define _XFMT_PAD               13
#define _XFMT_RVRS              15
#define _XFMT_DATDLY            16

#define _RFMT_ROT                0
#define _RFMT_BUSEL              3
#define _RFMT_SSZ                4
#define _RFMT_PBIT               8
#define _RFMT_PAD               13
#define _RFMT_RVRS              15
#define _RFMT_DATDLY            16

//Receive and Transmit Frame Sync Control Registers 
#define MCASP_AFSRCTL(n)        *(volatile unsigned int*)(MCASP(n)+0x006C)
#define MCASP_AFSXCTL(n)        *(volatile unsigned int*)(MCASP(n)+0x00AC)

#define AFSXCTL_FSXP            0x00000001
#define AFSXCTL_FSXM            0x00000002
#define AFSXCTL_FXWID           0x00000010
#define AFSXCTL_XMOD(x)         ((x)<<7)

#define AFSRCTL_FSRP            0x00000001
#define AFSRCTL_FSRM            0x00000002
#define AFSRCTL_FRWID           0x00000010
#define AFSRCTL_RMOD(x)         ((x)<<7)

#define _AFSXCTL_FSXP           0
#define _AFSXCTL_FSXM           1
#define _AFSXCTL_FXWID          4
#define _AFSXCTL_XMOD           7

#define _AFSRCTL_FSRP           0
#define _AFSRCTL_FSRM           1
#define _AFSRCTL_FRWID          4
#define _AFSRCTL_RMOD           7

//Receive and Transmit Clock Control Registers
#define MCASP_ACLKRCTL(n)       *(volatile unsigned int*)(MCASP(n)+0x0070)
#define MCASP_ACLKXCTL(n)       *(volatile unsigned int*)(MCASP(n)+0x00B0)

#define ACLKXCTL_CLKXDIV(x)     (x)
#define ACLKXCTL_CLKXM          0x00000020
#define ACLKXCTL_ASYNC          0x00000040
#define ACLKXCTL_CLKXP          0x00000080

#define ACLKRCTL_CLKRDIV(x)     (x)
#define ACLKRCTL_CLKRM          0x00000020
#define ACLKRCTL_ASYNC          0x00000040
#define ACLKRCTL_CLKRP          0x00000080

#define _ACLKXCTL_CLKXDIV       0
#define _ACLKXCTL_CLKXM         5
#define _ACLKXCTL_ASYNC         6
#define _ACLKXCTL_CLKXP         7

#define _ACLKRCTL_CLKRDIV       0
#define _ACLKRCTL_CLKRM         5
#define _ACLKRCTL_ASYNC         6
#define _ACLKRCTL_CLKRP         7

//Receive and Transmit  High-Frequency Clock Control Registers
#define MCASP_AHCLKRCTL(n)      *(volatile unsigned int*)(MCASP(n)+0x0074)
#define MCASP_AHCLKXCTL(n)      *(volatile unsigned int*)(MCASP(n)+0x00B4)

#define AHCLKXCTL_HCLKXDIV(x)   (x)
#define AHCLKXCTL_HCLKXP        0x00004000
#define AHCLKXCTL_HCLKXM        0x00008000

#define AHCLKRCTL_HCLKRDIV(x)   (x)
#define AHCLKRCTL_HCLKRP        0x00004000
#define AHCLKRCTL_HCLKRM        0x00008000

#define _AHCLKXCTL_HCLKXDIV      0
#define _AHCLKXCTL_HCLKXP       14
#define _AHCLKXCTL_HCLKXM       15

#define _AHCLKRCTL_HCLKRDIV      0
#define _AHCLKRCTL_HCLKRP       14
#define _AHCLKRCTL_HCLKRM       15

//Receive and Transmit TDM Time Slot Registers
#define MCASP_RTDM(n)           *(volatile unsigned int*)(MCASP(n)+0x0078)
#define MCASP_XTDM(n)           *(volatile unsigned int*)(MCASP(n)+0x00B8)

//Receiver and Transmitter Interrupt Control Registers
#define MCASP_RINTCTL(n)        *(volatile unsigned int*)(MCASP(n)+0x007C)
#define MCASP_XINTCTL(n)        *(volatile unsigned int*)(MCASP(n)+0x00BC)

#define XINTCTL_XUNDRN          0x00000001
#define XINTCTL_XSYNCERR        0x00000002
#define XINTCTL_XCKFAIL         0x00000004
#define XINTCTL_XDMAERR         0x00000008
#define XINTCTL_XLAST           0x00000010
#define XINTCTL_XDATA           0x00000020
#define XINTCTL_XSTAFRM         0x00000080

#define RINTCTL_RUNDRN          0x00000001
#define RINTCTL_RSYNCERR        0x00000002
#define RINTCTL_RCKFAIL         0x00000004
#define RINTCTL_RDMAERR         0x00000008
#define RINTCTL_RLAST           0x00000010
#define RINTCTL_RDATA           0x00000020
#define RINTCTL_RSTAFRM         0x00000080

#define _XINTCTL_XUNDRN         0
#define _XINTCTL_XSYNCERR       1
#define _XINTCTL_XCKFAIL        2
#define _XINTCTL_XDMAERR        3
#define _XINTCTL_XLAST          4
#define _XINTCTL_XDATA          5
#define _XINTCTL_XSTAFRM        7

#define _RINTCTL_RUNDRN         0
#define _RINTCTL_RSYNCERR       1
#define _RINTCTL_RCKFAIL        2
#define _RINTCTL_RDMAERR        3
#define _RINTCTL_RLAST          4
#define _RINTCTL_RDATA          5
#define _RINTCTL_RSTAFRM        7

//Receiver and Transmitter Status Register
#define MCASP_RSTAT(n)          *(volatile unsigned int*)(MCASP(n)+0x0080)
#define MCASP_XSTAT(n)          *(volatile unsigned int*)(MCASP(n)+0x00C0)

#define _STAT_OVUNDRN		0
#define _STAT_SYNCERR		1
#define _STAT_CKFAIL		2
#define _STAT_TDMSLOT		3
#define _STAT_LAST			4
#define _STAT_DATA			5
#define _STAT_STAFRM		6
#define _STAT_DMAERR 		7
#define _STAT_ERR			8

//Current Receive and Transmit TDM Slot Registers
#define MCASP_RSLOT(n)          *(volatile unsigned int*)(MCASP(n)+0x0084)
#define MCASP_XSLOT(n)          *(volatile unsigned int*)(MCASP(n)+0x00C4)

#define _SLOT_SLOTCNT           0

//Receive and Transmit Clock Check Control Registers
#define MCASP_RCLKCHK(n)        *(volatile unsigned int*)(MCASP(n)+0x0088)
#define MCASP_XCLKCHK(n)        *(volatile unsigned int*)(MCASP(n)+0x00C8)

#define XCLKCHK_XPS(x)          (x)
#define XCLKCHK_XPS_1           0x00000000
#define XCLKCHK_XPS_2           0x00000001
#define XCLKCHK_XPS_4           0x00000002
#define XCLKCHK_XPS_8           0x00000003
#define XCLKCHK_XPS_16          0x00000004
#define XCLKCHK_XPS_32          0x00000005
#define XCLKCHK_XPS_64          0x00000006
#define XCLKCHK_XPS_128         0x00000007
#define XCLKCHK_XPS_256         0x00000008
#define XCLKCHK_XCKFAILSW       0x00000080
#define XCLKCHK_XMIN(x)         ((x)<<8)
#define XCLKCHK_XMAX(x)         ((x)<<16)
#define XCLKCHK_XCNT(x)         ((x)<<24)

#define RCLKCHK_RPS(x)          (x)
#define RCLKCHK_RPS_1           0x00000000
#define RCLKCHK_RPS_2           0x00000001
#define RCLKCHK_RPS_4           0x00000002
#define RCLKCHK_RPS_8           0x00000003
#define RCLKCHK_RPS_16          0x00000004
#define RCLKCHK_RPS_32          0x00000005
#define RCLKCHK_RPS_64          0x00000006
#define RCLKCHK_RPS_128         0x00000007
#define RCLKCHK_RPS_256         0x00000008
#define RCLKCHK_RMIN(x)         ((x)<<8)
#define RCLKCHK_RMAX(x)         ((x)<<16)
#define RCLKCHK_RCNT(x)         ((x)<<24)

#define _XCLKCHK_XPS             0
#define _XCLKCHK_XCKFAILSW       7
#define _XCLKCHK_XMIN            8
#define _XCLKCHK_XMAX           16
#define _XCLKCHK_XCNT           24

#define _RCLKCHK_RPS             0
#define _RCLKCHK_RMIN            8
#define _RCLKCHK_RMAX           16
#define _RCLKCHK_RCNT           24

//Serializer Control Registers
#define MCASP_SRCTL(n, t)       *(volatile unsigned int*)(MCASP(n)+0x0180+(t)*4)

#define SRCTL_SRMOD(x)          (x)
#define SRCTL_DISMOD(x)         ((x)<<2)
#define SRCTL_XRDY              0x00000010
#define SRCTL_RRDY              0x00000020

#define _SRCTL_SRMOD            0
#define _SRCTL_DISMOD           2
#define _SRCTL_XRDY             4
#define _SRCTL_RRDY             5

//DIT Left Channel Status Registers 
#define MCASP_DITCSRA(n, t)     *(volatile unsigned int*)(MCASP(n)+0x0100+(t)*4)
//DIT Right Channel Status Registers 
#define MCASP_DITCSRB(n, t)     *(volatile unsigned int*)(MCASP(n)+0x0118+(t)*4)
//DIT Left Channel User Data Registers
#define MCASP_DITUDRA(n, t)     *(volatile unsigned int*)(MCASP(n)+0x0130+(t)*4)
//DIT Right Channel User Data Registers
#define MCASP_DITUDRB(n, t)     *(volatile unsigned int*)(MCASP(n)+0x0148+(t)*4)
//Transmit Buffer Registers
#define MCASP_XBUF(n, t)        *(volatile unsigned int*)(MCASP(n)+0x0200+(t)*4)
//Receive Buffer Registers
#define MCASP_RBUF(n, t)        *(volatile unsigned int*)(MCASP(n)+0x0280+(t)*4)
//
#define MCASP_XRBUF(n)          *(volatile unsigned int*)(0x3c000000 + (n)*0x00100000)
#define MCASP_RBUF_XBUF(n, r)   *(volatile unsigned int*)(0x3c000000 + (n)*0x00100000 + (r)*4)
/*******************************************************************************
 EDMA Registers
 *******************************************************************************/
// EDMA Parameter
#define EDMA_PARM_BASE          0x01A00000
#define EDMA_OPT(n)             *(volatile unsigned int*)(EDMA_PARM_BASE + (n)*24 + 0x00)
#define EDMA_SRC(n)             *(volatile unsigned int*)(EDMA_PARM_BASE + (n)*24 + 0x04)
#define EDMA_CNT(n)             *(volatile unsigned int*)(EDMA_PARM_BASE + (n)*24 + 0x08)
#define EDMA_DST(n)             *(volatile unsigned int*)(EDMA_PARM_BASE + (n)*24 + 0x0C)
#define EDMA_IDX(n)             *(volatile unsigned int*)(EDMA_PARM_BASE + (n)*24 + 0x10)
#define EDMA_RLD(n)             *(volatile unsigned int*)(EDMA_PARM_BASE + (n)*24 + 0x14)
// EDMA Control Registers
#define EDMA_ESEL_ADDR          0x01A0FF00
#define EDMA_ESEL0              *(volatile unsigned int*)(0x01A0FF00)
#define EDMA_ESEL1              *(volatile unsigned int*)(0x01A0FF04)
#define EDMA_ESEL3              *(volatile unsigned int*)(0x01A0FF0C)
#define EDMA_PQSR               *(volatile unsigned int*)(0x01A0FFE0)
#define EDMA_CIPR               *(volatile unsigned int*)(0x01A0FFE4)
#define EDMA_CIER               *(volatile unsigned int*)(0x01A0FFE8)
#define EDMA_CCER               *(volatile unsigned int*)(0x01A0FFEC)
#define EDMA_ER                 *(volatile unsigned int*)(0x01A0FFF0)
#define EDMA_EER                *(volatile unsigned int*)(0x01A0FFF4)
#define EDMA_ECR                *(volatile unsigned int*)(0x01A0FFF8)
#define EDMA_ESR                *(volatile unsigned int*)(0x01A0FFFC)

#define EDMA_OPT_FS             0x00000001
#define EDMA_OPT_LINK           0x00000002
#define EDMA_OPT_TCC(x)         ((x)<<16)
#define EDMA_OPT_TCINT          0x00100000
#define EDMA_OPT_DUM(x)         ((x)<<21)
#define EDMA_OPT_DUM_NONE       0x00000000
#define EDMA_OPT_DUM_INC        0x00200000
#define EDMA_OPT_DUM_DEC        0x00400000
#define EDMA_OPT_DUM_IDX        0x00600000
#define EDMA_OPT_2DD            0x00800000
#define EDMA_OPT_SUM(x)         ((x)<<24)
#define EDMA_OPT_SUM_NONE       0x00000000
#define EDMA_OPT_SUM_INC        0x01000000
#define EDMA_OPT_SUM_DEC        0x02000000
#define EDMA_OPT_SUM_IDX        0x03000000
#define EDMA_OPT_2DS            0x04000000
#define EDMA_OPT_ESIZE(x)       ((x)<<27)
#define EDMA_OPT_ESIZE_32       0x00000000
#define EDMA_OPT_ESIZE_16       0x08000000
#define EDMA_OPT_ESIZE_8        0x10000000
#define EDMA_OPT_PRI(x)         ((x)<<29)
#define EDMA_OPT_PRI_HIGH       0x20000000
#define EDMA_OPT_PRI_LOW        0x40000000

#define _EDMA_OPT_FS             0
#define _EDMA_OPT_LINK           1
#define _EDMA_OPT_TCC           16
#define _EDMA_OPT_TCINT         20
#define _EDMA_OPT_DUM           21
#define _EDMA_OPT_2DD           23
#define _EDMA_OPT_SUM           24
#define _EDMA_OPT_2DS           26
#define _EDMA_OPT_ESIZE         27
#define _EDMA_OPT_PRI           29

#define EDMA_CNT_ELECNT(x)      (x)
#define EDMA_CNT_FRMCNT(x)      ((x)<<16)

#define _EDMA_CNT_ELECNT         0
#define _EDMA_CNT_FRMCNT        16

#define EDMA_IDX_ELEIDX(x)      (x)
#define EDMA_IDX_FRMIDX(x)      ((x)<<16)

#define _EDMA_IDX_ELEIDX         0
#define _EDMA_IDX_FRMIDX        16

#define EDMA_RLD_LINK(x)        (x)
#define EDMA_RLD_ELERLD(x)      ((x)<<16)

#define _EDMA_RLD_LINK           0
#define _EDMA_RLD_ELERLD        16

#define ESEL0_EVT0(x)           (x)
#define ESEL0_EVT1(x)           ((x)<<8)
#define ESEL0_EVT2(x)           ((x)<<16)
#define ESEL0_EVT3(x)           ((x)<<24)

#define _ESEL0_EVT0              0
#define _ESEL0_EVT1              8
#define _ESEL0_EVT2             16
#define _ESEL0_EVT3             24

#define ESEL1_EVT4(x)           (x)
#define ESEL1_EVT5(x)           ((x)<<8)
#define ESEL1_EVT6(x)           ((x)<<16)
#define ESEL1_EVT7(x)           ((x)<<24)

#define _ESEL1_EVT4              0
#define _ESEL1_EVT5              8
#define _ESEL1_EVT6             16
#define _ESEL1_EVT7             24

#define ESEL3_EVT12(x)          (x)
#define ESEL3_EVT13(x)          ((x)<<8)
#define ESEL3_EVT14(x)          ((x)<<16)
#define ESEL3_EVT15(x)          ((x)<<24)

#define _ESEL3_EVT12             0
#define _ESEL3_EVT13             8
#define _ESEL3_EVT14            16
#define _ESEL3_EVT15            24

// Event selector
#define EDMA_ESEL_EVTSEL(n, v)     {                  \
  unsigned int reg;                                   \
  reg = *(unsigned int *)(EDMA_ESEL_ADDR + 4*((n)/4));\
  reg &= ~(0x3F << (((n)&0x03)*8));                   \
  reg |= (v) << (((n)&0x03)*8);                       \
  *(unsigned int *)(EDMA_ESEL_ADDR + 4*((n)/4)) = reg;\
                                   }


#define EDMA_DSPINT             0x00
#define EDMA_TINT0              0x01
#define EDMA_TINT1              0x02
#define EDMA_SDINT              0x03
#define EDMA_GPINT4             0x04
#define EDMA_GPINT5             0x05
#define EDMA_GPINT6             0x06
#define EDMA_GPINT7             0x07
#define EDMA_GPINT0             0x08
#define EDMA_GPINT1             0x09
#define EDMA_GPINT2             0x0A
#define EDMA_GPINT3             0x0B
#define EDMA_XEVT0              0x0C
#define EDMA_REVT0              0x0D
#define EDMA_XEVT1              0x0E
#define EDMA_REVT1              0x0F
#define EDMA_AXEVTE0            0x20
#define EDMA_AXEVTO0            0x21
#define EDMA_AXEVT0             0x22
#define EDMA_AREVTE0            0x23
#define EDMA_AREVTO0            0x24
#define EDMA_AREVT0             0x25
#define EDMA_AXEVTE1            0x26
#define EDMA_AXEVTO1            0x27
#define EDMA_AXEVT1             0x28
#define EDMA_AREVTE1            0x29
#define EDMA_AREVTO1            0x2A
#define EDMA_AREVT1             0x2B
#define EDMA_I2CREVT0           0x2C
#define EDMA_I2CXEVT0           0x2D
#define EDMA_I2CREVT1           0x2E
#define EDMA_I2CXEVT1           0x2F
#define EDMA_GPINT8             0x30
#define EDMA_GPINT9             0x31
#define EDMA_GPINT10            0x32
#define EDMA_GPINT11            0x33
#define EDMA_GPINT12            0x34
#define EDMA_GPINT13            0x35
#define EDMA_GPINT14            0x36
#define EDMA_GPINT15            0x37


#endif // _TMS320C67X_H_
