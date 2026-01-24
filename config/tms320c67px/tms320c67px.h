#ifndef _TMS320C67PX_H_
#define _TMS320C67PX_H_


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
extern cregister volatile unsigned int AMR;  //Addressing mode register
extern cregister volatile unsigned int CSR;  //Control status register
extern cregister volatile unsigned int DETR; //d MAX register
extern cregister volatile unsigned int DESR; //d MAX register

#define _CSR_GIE        0
#define _CSR_PGIE       1
#define _CSR_DCC        2
#define _CSR_PCC        5
#define _CSR_EN         8
#define _CSR_SAT        9
#define _CSR_PWRD       10
#define _CSR_REVID      16
#define _CSR_CPUID      24

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
#define INTR_CLEAR(n)   ICR  = 0x1<<(n)

/*******************************************************************************
 Interrupt Selector Registers
 *******************************************************************************/
//Interrupt event
#define IEV_DSPINT      0x00 //HPI
#define IEV_TINT0       0x01 //Timer 0
#define IEV_TINT1       0x02 //Timer 1
#define IEV_SDINT       0x03 //EMIF 
#define IEV_GPINT4      0x04 //GPIO   
#define IEV_GPINT5      0x05 //GPIO   
#define IEV_GPINT6      0x06 //GPIO   
#define IEV_GPINT7      0x07 //GPIO   
#define IEV_EDMAINT     0x08 //EDMA   
#define IEV_EMUDTDMA    0x09 //Emulation    
#define IEV_EMURTDXRX   0x0A //Emulation     
#define IEV_EMURTDXTX   0x0B //Emulation     
#define IEV_XINT0       0x0C //McBSP0 
#define IEV_RINT0       0x0D //McBSP0 
#define IEV_XINT1       0x0E //McBSP1 
#define IEV_RINT1       0x0F //McBSP1 
#define IEV_GPINT0      0x10 //GPIO  
#define IEV_I2CINT0     0x16 //I2C0   
#define IEV_I2CINT1     0x17 //I2C1   
#define IEV_AXINT0      0x1C //McASP0  
#define IEV_ARINT0      0x1D //McASP0  
#define IEV_AXINT1      0x1E //McASP1  
#define IEV_ARINT1      0x1F //McASP1  
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

/*******************************************************************************
 Timers Registers / RTI module registers
 *******************************************************************************/
#define CFGRTI          *(volatile unsigned int*)(0x40000014)

#define RTIGCTRL        *(volatile unsigned int*)(0x42000000)
#define RTICAPCTRL      *(volatile unsigned int*)(0x42000008)
#define RTICOMPCTRL     *(volatile unsigned int*)(0x4200000C)
#define RTIFRC0         *(volatile unsigned int*)(0x42000010)
#define RTIUC0          *(volatile unsigned int*)(0x42000014)
#define RTICPUC0        *(volatile unsigned int*)(0x42000018)
#define RTICAFRC0       *(volatile unsigned int*)(0x42000020)
#define RTICAUC0        *(volatile unsigned int*)(0x42000024)
#define RTIFRC1         *(volatile unsigned int*)(0x42000030)
#define RTIUC1          *(volatile unsigned int*)(0x42000034)
#define RTICPUC1        *(volatile unsigned int*)(0x42000038)
#define RTICAFRC1       *(volatile unsigned int*)(0x42000040)
#define RTICAUC1        *(volatile unsigned int*)(0x42000044)
#define RTICOMP0        *(volatile unsigned int*)(0x42000050)
#define RTIUDCP0        *(volatile unsigned int*)(0x42000054)
#define RTICOMP1        *(volatile unsigned int*)(0x42000058)
#define RTIUDCP1        *(volatile unsigned int*)(0x4200005C)
#define RTICOMP2        *(volatile unsigned int*)(0x42000060)
#define RTIUDCP2        *(volatile unsigned int*)(0x42000064)
#define RTICOMP3        *(volatile unsigned int*)(0x42000068)
#define RTIUDCP3        *(volatile unsigned int*)(0x4200006C)
#define RTISETINT       *(volatile unsigned int*)(0x42000080)
#define RTICLEARINT     *(volatile unsigned int*)(0x42000084)
#define RTIINTFLAG      *(volatile unsigned int*)(0x42000088)
#define RTIDWDCTRL      *(volatile unsigned int*)(0x42000090)
#define RTIDWDPRLD      *(volatile unsigned int*)(0x42000094)
#define RTIWDSTATUS     *(volatile unsigned int*)(0x42000098)
#define RTIWDKEY        *(volatile unsigned int*)(0x4200009C)
#define RTIDWDCNTR      *(volatile unsigned int*)(0x420000A0)
 
// 
#define ENABLE_RTI0_INT()      INTR_ENABLE(4)
#define DISABLE_RTI0_INT()     INTR_DISABLE(4)

/*******************************************************************************
 PLL Controller Registers 
 *******************************************************************************/
#define PLLPID          *(volatile unsigned int*)(0x41000000)
#define PLLCSR          *(volatile unsigned int*)(0x41000100)
#define PLLM            *(volatile unsigned int*)(0x41000110)
#define PLLDIV0         *(volatile unsigned int*)(0x41000114)
#define PLLDIV1         *(volatile unsigned int*)(0x41000118)
#define PLLDIV2         *(volatile unsigned int*)(0x4100011C)
#define PLLDIV3         *(volatile unsigned int*)(0x41000120)
#define PLLCMD          *(volatile unsigned int*)(0x41000138)
#define PLLSTAT         *(volatile unsigned int*)(0x4100013C)
#define ALNCTL          *(volatile unsigned int*)(0x41000140)
#define CKEN            *(volatile unsigned int*)(0x41000148)
#define CKSTAT          *(volatile unsigned int*)(0x4100014C)
#define SYSTAT          *(volatile unsigned int*)(0x41000150)

#define PLLCSR_PLLEN    0x01
#define PLLCSR_PLLPWRDN 0x02
#define PLLCSR_PLLRST   0x08
#define PLLCSR_STABLE   0x40

#define PLLDIV_D_EN     0x00008000
/*******************************************************************************
 Device Configuration
 *******************************************************************************/
#define CFGPIN0     *(volatile unsigned int*)0x40000000
#define CFGPIN1     *(volatile unsigned int*)0x40000004
#define CFGHPI      *(volatile unsigned int*)0x40000008
#define CFGHPIAMSB  *(volatile unsigned int*)0x4000000C
#define CFGHPIAUMB  *(volatile unsigned int*)0x40000010

#define CFGMCASP1   *(volatile unsigned int*)0x4000001C
#define CFGMCASP2   *(volatile unsigned int*)0x40000020
#define CFGBRIDGE   *(volatile unsigned int*)0x40000024

/*******************************************************************************
 Cache Configuration
 *******************************************************************************/
#define L1PSAR *(volatile unsigned int *)(0x20000000)
#define L1PICR *(volatile unsigned int *)(0x20000004)
#define L1P_INVALIDATE (0x80000000)
#define PCC_ENABLE0 (0x00000000)
#define PCC_ENABLE (0x00000040)
#define PCC_FREEZE (0x00000060)
#define PCC_BYPASS (0x00000080)
#define PCC_MASK (0x000000E0)

/*******************************************************************************
 EMIF Registers
 *******************************************************************************/
#define AWCCR       *(volatile unsigned int*)0xF0000004
#define SDCR        *(volatile unsigned int*)0xF0000008
#define SDRCR       *(volatile unsigned int*)0xF000000C
#define A1CR        *(volatile unsigned int*)0xF0000010
#define SDTIMR      *(volatile unsigned int*)0xF0000020
#define SDSRETR     *(volatile unsigned int*)0xF000003C
#define EIRR        *(volatile unsigned int*)0xF0000040
#define EIMR        *(volatile unsigned int*)0xF0000044
#define EIMSR       *(volatile unsigned int*)0xF0000048
#define EIMCR       *(volatile unsigned int*)0xF000004C
#define NANDFCR     *(volatile unsigned int*)0xF0000060
#define NANDFSR     *(volatile unsigned int*)0xF0000064
#define NANDF1ECC   *(volatile unsigned int*)0xF0000070
 

#define ASIZE_8BIT              0x00000000
#define ASIZE_16BIT             0x00000001
#define ASIZE_32BIT             0x00000002

#define TA(x)                   ((x)<<2)
#define R_HOLD(x)               ((x)<<4)
#define R_STROBE(x)             ((x)<<7)
#define R_SETUP(x)              ((x)<<13)
#define W_HOLD(x)               ((x)<<17)
#define W_STROBE(x)             ((x)<<20)
#define W_SETUP(x)              ((x)<<26)

#define WAIT_CYCLES             0x40000000
#define SELECT_STROBE_MODE      0x80000000



#define CECTL_RDHLD(x)          (x)
#define CECTL_MTYPE(x)          ((x)<<4)
#define CECTL_RDSTRB(x)         ((x)<<8)
#define CECTL_TA(x)             ((x)<<14)
#define CECTL_RDSETUP(x)        ((x)<<16)
#define CECTL_WRHLD(x)          ((x)<<20)
#define CECTL_WRSTRB(x)         ((x)<<22)
#define CECTL_WRSETUP(x)        ((x)<<28)

/*******************************************************************************
 I2C Registers
 *******************************************************************************/

//I2C0 Registers
#define I2COAR0         *(volatile unsigned int*)(0x49000000)   //I2C0 own address register
#define I2CIER0         *(volatile unsigned int*)(0x49000004)   //I2C0 interrupt enable register
#define I2CSTR0         *(volatile unsigned int*)(0x49000008)   //I2C0 interrupt status register
#define I2CCLKL0        *(volatile unsigned int*)(0x4900000C)   //I2C0 clock low-time divider register
#define I2CCLKH0        *(volatile unsigned int*)(0x49000010)   //I2C0 clock high-time divider register
#define I2CCNT0         *(volatile unsigned int*)(0x49000014)   //I2C0 data count register
#define I2CDRR0         *(volatile unsigned int*)(0x49000018)   //I2C0 data receive register
#define I2CSAR0         *(volatile unsigned int*)(0x4900001C)   //I2C0 slave address register
#define I2CDXR0         *(volatile unsigned int*)(0x49000020)   //I2C0 data transmit register
#define I2CMDR0         *(volatile unsigned int*)(0x49000024)   //I2C0 mode register
#define I2CISRC0        *(volatile unsigned int*)(0x49000028)   //I2C0 interrupt source register
#define I2CEMDR0        *(volatile unsigned int*)(0x4900002C)   //I2C0 Extended Mode Register
#define I2CPSC0         *(volatile unsigned int*)(0x49000030)   //I2C0 prescaler register
#define I2CPID1_0       *(volatile unsigned int*)(0x49000034)   //I2C0 Peripheral Identification Register 1
#define I2CPID2_0       *(volatile unsigned int*)(0x49000038)   //I2C0 Peripheral Identification Register 2
#define I2CPFUNC0       *(volatile unsigned int*)(0x49000048)   //I2C0 Pin Function Register
#define I2CPDIR0        *(volatile unsigned int*)(0x4900004C)   //I2C0 Pin Direction Register
#define I2CPDIN0        *(volatile unsigned int*)(0x49000050)   //I2C0 Pin Data Input Register
#define I2CPDOUT0       *(volatile unsigned int*)(0x49000054)   //I2C0 Pin Data Output Register
#define I2CPDSET0       *(volatile unsigned int*)(0x49000058)   //I2C0 Pin Data Set Register
#define I2CPDCLR0       *(volatile unsigned int*)(0x4900005C)   //I2C0 Pin Data Clear Register

//I2C1 Registers
#define I2COAR1         *(volatile unsigned int*)(0x4A000000)   //I2C1 own address register
#define I2CIER1         *(volatile unsigned int*)(0x4A000004)   //I2C1 interrupt enable register
#define I2CSTR1         *(volatile unsigned int*)(0x4A000008)   //I2C1 interrupt status register
#define I2CCLKL1        *(volatile unsigned int*)(0x4A00000C)   //I2C1 clock low-time divider register
#define I2CCLKH1        *(volatile unsigned int*)(0x4A000010)   //I2C1 clock high-time divider register
#define I2CCNT1         *(volatile unsigned int*)(0x4A000014)   //I2C1 data count register
#define I2CDRR1         *(volatile unsigned int*)(0x4A000018)   //I2C1 data receive register
#define I2CSAR1         *(volatile unsigned int*)(0x4A00001C)   //I2C1 slave address register
#define I2CDXR1         *(volatile unsigned int*)(0x4A000020)   //I2C1 data transmit register
#define I2CMDR1         *(volatile unsigned int*)(0x4A000024)   //I2C1 mode register
#define I2CISRC1        *(volatile unsigned int*)(0x4A000028)   //I2C1 interrupt source register
#define I2CEMDR1        *(volatile unsigned int*)(0x4A00002C)   //I2C1 Extended Mode Register
#define I2CPSC1         *(volatile unsigned int*)(0x4A000030)   //I2C1 prescaler register
#define I2CPID1_1       *(volatile unsigned int*)(0x4A000034)   //I2C1 Peripheral Identification Register 1
#define I2CPID2_1       *(volatile unsigned int*)(0x4A000038)   //I2C1 Peripheral Identification Register 2
#define I2CPFUNC1       *(volatile unsigned int*)(0x4A000048)   //I2C1 Pin Function Register
#define I2CPDIR1        *(volatile unsigned int*)(0x4A00004C)   //I2C1 Pin Direction Register
#define I2CPDIN1        *(volatile unsigned int*)(0x4A000050)   //I2C1 Pin Data Input Register
#define I2CPDOUT1       *(volatile unsigned int*)(0x4A000054)   //I2C1 Pin Data Output Register
#define I2CPDSET1       *(volatile unsigned int*)(0x4A000058)   //I2C1 Pin Data Set Register
#define I2CPDCLR1       *(volatile unsigned int*)(0x4A00005C)   //I2C1 Pin Data Clear Register

//I2C ~ I2C0
#define I2COAR          I2COAR1
//I2C Interrupt Enable Register
#define I2CIER          I2CIER1
#define I2CIER_AL       0x01
#define I2CIER_NACK     0x02
#define I2CIER_ARDY     0x04
#define I2CIER_ICRRDY   0x08
#define I2CIER_RRDY     I2CIER_ICRRDY
#define I2CIER_ICXRDY   0x10
#define I2CIER_XRDY     I2CIER_ICXRDY
#define I2CIER_SCD      0x20

//I2C Status Register
#define I2CSTR          I2CSTR1
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
#define I2CCLKL         I2CCLKL1
#define I2CCLKH         I2CCLKH1
#define I2CCNT          I2CCNT1 
#define I2CDRR          I2CDRR1 
#define I2CSAR          I2CSAR1 
#define I2CDXR          I2CDXR1 
//I2C Mode Register
#define I2CMDR          I2CMDR1
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
#define I2CISRC                 I2CISRC1
#define I2CISRC_INTCODE         0x07
#define I2CISRC_INTCODE_ARDY    0x3
#define I2CISRC_INTCODE_RRDY    0x4
#define I2CISRC_INTCODE_XRDY    0x5 
#define I2CISRC_INTCODE_SCD     0x6
//
#define I2CPSC          I2CPSC1 
//
#define I2CEMDR         I2CEMDR0

/*******************************************************************************
 McASP0 and McASP1 Registers
 *******************************************************************************/
// McASP0
#define CFGMCASP0   *(volatile unsigned int*)0x40000018

#define PID0        *(volatile unsigned int*)0x44000000
#define PWRDEMU0    *(volatile unsigned int*)0x44000004
#define PFUNC0      *(volatile unsigned int*)0x44000010
#define PDIR0       *(volatile unsigned int*)0x44000014
#define PDOUT0      *(volatile unsigned int*)0x44000018
#define PDIN0       *(volatile unsigned int*)0x4400001C
#define PDSET0      *(volatile unsigned int*)0x4400001C
#define PDCLR0      *(volatile unsigned int*)0x44000020

#define GBLCTL0     *(volatile unsigned int*)0x44000044
#define AMUTE0      *(volatile unsigned int*)0x44000048
#define DLBCTL0     *(volatile unsigned int*)0x4400004C
#define DITCTL0     *(volatile unsigned int*)0x44000050
#define RGBLCTL0    *(volatile unsigned int*)0x44000060
#define RMASK0      *(volatile unsigned int*)0x44000064
#define RFMT0       *(volatile unsigned int*)0x44000068
#define AFSRCTL0    *(volatile unsigned int*)0x4400006C
#define ACLKRCTL0   *(volatile unsigned int*)0x44000070
#define AHCLKRCTL0  *(volatile unsigned int*)0x44000074
#define RTDM0       *(volatile unsigned int*)0x44000078
#define RINTCTL0    *(volatile unsigned int*)0x4400007C
#define RSTAT0      *(volatile unsigned int*)0x44000080
#define RSLOT0      *(volatile unsigned int*)0x44000084
#define RCLKCHK0    *(volatile unsigned int*)0x44000088
#define REVTCTL0    *(volatile unsigned int*)0x4400008C
#define XGBLCTL0    *(volatile unsigned int*)0x440000A0
#define XMASK0      *(volatile unsigned int*)0x440000A4
#define XFMT0       *(volatile unsigned int*)0x440000A8
#define AFSXCTL0    *(volatile unsigned int*)0x440000AC
#define ACLKXCTL0   *(volatile unsigned int*)0x440000B0
#define AHCLKXCTL0  *(volatile unsigned int*)0x440000B4
#define XTDM0       *(volatile unsigned int*)0x440000B8
#define XINTCTL0    *(volatile unsigned int*)0x440000BC
#define XSTAT0      *(volatile unsigned int*)0x440000C0
#define XSLOT0      *(volatile unsigned int*)0x440000C4
#define XCLKCHK0    *(volatile unsigned int*)0x440000C8
#define XEVTCTL0    *(volatile unsigned int*)0x440000CC

#define SRCTL0_0    *(volatile unsigned int*)0x44000180
#define SRCTL0_1    *(volatile unsigned int*)0x44000184
#define SRCTL0_2    *(volatile unsigned int*)0x44000188
#define SRCTL0_3    *(volatile unsigned int*)0x4400018C
#define SRCTL0_4    *(volatile unsigned int*)0x44000190
#define SRCTL0_5    *(volatile unsigned int*)0x44000194
#define SRCTL0_6    *(volatile unsigned int*)0x44000198
#define SRCTL0_7    *(volatile unsigned int*)0x4400019C
#define SRCTL0_8    *(volatile unsigned int*)0x440001A0
#define SRCTL0_9    *(volatile unsigned int*)0x440001A4
#define SRCTL0_10   *(volatile unsigned int*)0x440001A8
#define SRCTL0_11   *(volatile unsigned int*)0x440001AC
#define SRCTL0_12   *(volatile unsigned int*)0x440001B0
#define SRCTL0_13   *(volatile unsigned int*)0x440001B4
#define SRCTL0_14   *(volatile unsigned int*)0x440001B8
#define SRCTL0_15   *(volatile unsigned int*)0x440001BC

#define XBUF0_0     *(volatile unsigned int*)0x44000200
#define XBUF0_1     *(volatile unsigned int*)0x44000204
#define XBUF0_2     *(volatile unsigned int*)0x44000208
#define XBUF0_3     *(volatile unsigned int*)0x4400020C
#define XBUF0_4     *(volatile unsigned int*)0x44000210
#define XBUF0_5     *(volatile unsigned int*)0x44000214
#define XBUF0_6     *(volatile unsigned int*)0x44000218
#define XBUF0_7     *(volatile unsigned int*)0x4400021C
#define XBUF0_8     *(volatile unsigned int*)0x44000220
#define XBUF0_9     *(volatile unsigned int*)0x44000224
#define XBUF0_10    *(volatile unsigned int*)0x44000228
#define XBUF0_11    *(volatile unsigned int*)0x4400022C
#define XBUF0_12    *(volatile unsigned int*)0x44000230
#define XBUF0_13    *(volatile unsigned int*)0x44000234
#define XBUF0_14    *(volatile unsigned int*)0x44000238
#define XBUF0_15    *(volatile unsigned int*)0x4400023C

#define RBUF0_0     *(volatile unsigned int*)0x44000280
#define RBUF0_1     *(volatile unsigned int*)0x44000284
#define RBUF0_2     *(volatile unsigned int*)0x44000288
#define RBUF0_3     *(volatile unsigned int*)0x4400028C
#define RBUF0_4     *(volatile unsigned int*)0x44000290
#define RBUF0_5     *(volatile unsigned int*)0x44000294
#define RBUF0_6     *(volatile unsigned int*)0x44000298
#define RBUF0_7     *(volatile unsigned int*)0x4400029C
#define RBUF0_8     *(volatile unsigned int*)0x440002A0
#define RBUF0_9     *(volatile unsigned int*)0x440002A4
#define RBUF0_10    *(volatile unsigned int*)0x440002A8
#define RBUF0_11    *(volatile unsigned int*)0x440002AC
#define RBUF0_12    *(volatile unsigned int*)0x440002B0
#define RBUF0_13    *(volatile unsigned int*)0x440002B4
#define RBUF0_14    *(volatile unsigned int*)0x440002B8
#define RBUF0_15    *(volatile unsigned int*)0x440002BC

// McASP1
#define CFGMCASP1   *(volatile unsigned int*)0x4000001C

#define PID1        *(volatile unsigned int*)0x45000000
#define PWRDEMU1    *(volatile unsigned int*)0x45000004
#define PFUNC1      *(volatile unsigned int*)0x45000010
#define PDIR1       *(volatile unsigned int*)0x45000014
#define PDOUT1      *(volatile unsigned int*)0x45000018
#define PDIN1       *(volatile unsigned int*)0x4500001C
#define PDSET1      *(volatile unsigned int*)0x4500001C
#define PDCLR1      *(volatile unsigned int*)0x45000020

#define GBLCTL1     *(volatile unsigned int*)0x45000044
#define AMUTE1      *(volatile unsigned int*)0x45000048
#define DLBCTL1     *(volatile unsigned int*)0x4500004C
#define DITCTL1     *(volatile unsigned int*)0x45000050
#define RGBLCTL1    *(volatile unsigned int*)0x45000060
#define RMASK1      *(volatile unsigned int*)0x45000064
#define RFMT1       *(volatile unsigned int*)0x45000068
#define AFSRCTL1    *(volatile unsigned int*)0x4500006C
#define ACLKRCTL1   *(volatile unsigned int*)0x45000070
#define AHCLKRCTL1  *(volatile unsigned int*)0x45000074
#define RTDM1       *(volatile unsigned int*)0x45000078
#define RINTCTL1    *(volatile unsigned int*)0x4500007C
#define RSTAT1      *(volatile unsigned int*)0x45000080
#define RSLOT1      *(volatile unsigned int*)0x45000084
#define RCLKCHK1    *(volatile unsigned int*)0x45000088
#define REVTCTL1    *(volatile unsigned int*)0x4500008C
#define XGBLCTL1    *(volatile unsigned int*)0x450000A0
#define XMASK1      *(volatile unsigned int*)0x450000A4
#define XFMT1       *(volatile unsigned int*)0x450000A8
#define AFSXCTL1    *(volatile unsigned int*)0x450000AC
#define ACLKXCTL1   *(volatile unsigned int*)0x450000B0
#define AHCLKXCTL1  *(volatile unsigned int*)0x450000B4
#define XTDM1       *(volatile unsigned int*)0x450000B8
#define XINTCTL1    *(volatile unsigned int*)0x450000BC
#define XSTAT1      *(volatile unsigned int*)0x450000C0
#define XSLOT1      *(volatile unsigned int*)0x450000C4
#define XCLKCHK1    *(volatile unsigned int*)0x450000C8
#define XEVTCTL1    *(volatile unsigned int*)0x450000CC

#define SRCTL1_0    *(volatile unsigned int*)0x45000180
#define SRCTL1_1    *(volatile unsigned int*)0x45000184
#define SRCTL1_2    *(volatile unsigned int*)0x45000188
#define SRCTL1_3    *(volatile unsigned int*)0x4500018C
#define SRCTL1_4    *(volatile unsigned int*)0x45000190
#define SRCTL1_5    *(volatile unsigned int*)0x45000194

#define XBUF1_0     *(volatile unsigned int*)0x45000200
#define XBUF1_1     *(volatile unsigned int*)0x45000204
#define XBUF1_2     *(volatile unsigned int*)0x45000208
#define XBUF1_3     *(volatile unsigned int*)0x4500020C
#define XBUF1_4     *(volatile unsigned int*)0x45000210
#define XBUF1_5     *(volatile unsigned int*)0x45000214

#define RBUF1_0     *(volatile unsigned int*)0x45000280
#define RBUF1_1     *(volatile unsigned int*)0x45000284
#define RBUF1_2     *(volatile unsigned int*)0x45000288
#define RBUF1_3     *(volatile unsigned int*)0x4500028C
#define RBUF1_4     *(volatile unsigned int*)0x45000290
#define RBUF1_5     *(volatile unsigned int*)0x45000294



// McASP2
#define CFGMCASP2   *(volatile unsigned int*)0x40000020

#define PID2        *(volatile unsigned int*)0x46000000
#define PWRDEMU2    *(volatile unsigned int*)0x46000004
#define PFUNC2      *(volatile unsigned int*)0x46000010
#define PDIR2       *(volatile unsigned int*)0x46000014
#define PDOUT2      *(volatile unsigned int*)0x46000018
#define PDIN2       *(volatile unsigned int*)0x4600001C
#define PDSET2      *(volatile unsigned int*)0x4600001C
#define PDCLR2      *(volatile unsigned int*)0x46000020

#define GBLCTL2     *(volatile unsigned int*)0x46000044
#define AMUTE2      *(volatile unsigned int*)0x46000048
#define DLBCTL2     *(volatile unsigned int*)0x4600004C
#define DITCTL2     *(volatile unsigned int*)0x46000050
#define RGBLCTL2    *(volatile unsigned int*)0x46000060
#define RMASK2      *(volatile unsigned int*)0x46000064
#define RFMT2       *(volatile unsigned int*)0x46000068
#define AFSRCTL2    *(volatile unsigned int*)0x4600006C
#define ACLKRCTL2   *(volatile unsigned int*)0x46000070
#define AHCLKRCTL2  *(volatile unsigned int*)0x46000074
#define RTDM2       *(volatile unsigned int*)0x46000078
#define RINTCTL2    *(volatile unsigned int*)0x4600007C
#define RSTAT2      *(volatile unsigned int*)0x46000080
#define RSLOT2      *(volatile unsigned int*)0x46000084
#define RCLKCHK2    *(volatile unsigned int*)0x46000088
#define REVTCTL2    *(volatile unsigned int*)0x4600008C
#define XGBLCTL2    *(volatile unsigned int*)0x460000A0
#define XMASK2      *(volatile unsigned int*)0x460000A4
#define XFMT2       *(volatile unsigned int*)0x460000A8
#define AFSXCTL2    *(volatile unsigned int*)0x460000AC
#define ACLKXCTL2   *(volatile unsigned int*)0x460000B0
#define AHCLKXCTL2  *(volatile unsigned int*)0x460000B4
#define XTDM2       *(volatile unsigned int*)0x460000B8
#define XINTCTL2    *(volatile unsigned int*)0x460000BC
#define XSTAT2      *(volatile unsigned int*)0x460000C0
#define XSLOT2      *(volatile unsigned int*)0x460000C4
#define XCLKCHK2    *(volatile unsigned int*)0x460000C8
#define XEVTCTL2    *(volatile unsigned int*)0x460000CC

#define SRCTL2_0    *(volatile unsigned int*)0x46000180
#define SRCTL2_1    *(volatile unsigned int*)0x46000184

#define XBUF2_0     *(volatile unsigned int*)0x46000200
#define XBUF2_1     *(volatile unsigned int*)0x46000204

#define RBUF2_0     *(volatile unsigned int*)0x46000280
#define RBUF2_1     *(volatile unsigned int*)0x46000284

#define DITCSRA0    *(volatile unsigned int*)0x46000100
#define DITCSRA1    *(volatile unsigned int*)0x46000104
#define DITCSRA2    *(volatile unsigned int*)0x46000108
#define DITCSRA3    *(volatile unsigned int*)0x4600010C
#define DITCSRA4    *(volatile unsigned int*)0x46000110
#define DITCSRA5    *(volatile unsigned int*)0x46000114
#define DITCSRB0    *(volatile unsigned int*)0x46000118
#define DITCSRB1    *(volatile unsigned int*)0x4600011C
#define DITCSRB2    *(volatile unsigned int*)0x46000120
#define DITCSRB3    *(volatile unsigned int*)0x46000124
#define DITCSRB4    *(volatile unsigned int*)0x46000128
#define DITCSRB5    *(volatile unsigned int*)0x4600012C
#define DITUDRA0    *(volatile unsigned int*)0x46000130
#define DITUDRA1    *(volatile unsigned int*)0x46000134
#define DITUDRA2    *(volatile unsigned int*)0x46000138
#define DITUDRA3    *(volatile unsigned int*)0x4600013C
#define DITUDRA4    *(volatile unsigned int*)0x46000140
#define DITUDRA5    *(volatile unsigned int*)0x46000144
#define DITUDRB0    *(volatile unsigned int*)0x46000148
#define DITUDRB1    *(volatile unsigned int*)0x4600014C
#define DITUDRB2    *(volatile unsigned int*)0x46000150
#define DITUDRB3    *(volatile unsigned int*)0x46000154
#define DITUDRB4    *(volatile unsigned int*)0x46000158
#define DITUDRB5    *(volatile unsigned int*)0x4600015C

//--
//--
//--
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
#define RFMT_RBUSEL             0x00000008
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

#define _STAT_OVUNDRN       0
#define _STAT_SYNCERR       1
#define _STAT_CKFAIL        2
#define _STAT_TDMSLOT       3
#define _STAT_LAST          4
#define _STAT_DATA          5
#define _STAT_STAFRM        6
#define _STAT_DMAERR        7
#define _STAT_ERR           8

#define _SLOT_SLOTCNT           0

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
dMAX Peripheral Registers
 *******************************************************************************/
#define DEPR                    *(volatile unsigned int*)0x60000008
#define DEER                    *(volatile unsigned int*)0x6000000C
#define DEDR                    *(volatile unsigned int*)0x60000010
#define DEHPR                   *(volatile unsigned int*)0x60000014
#define DELPR                   *(volatile unsigned int*)0x60000018
#define DEFR                    *(volatile unsigned int*)0x6000001C
#define DER0                    *(volatile unsigned int*)0x60000034
#define DER1                    *(volatile unsigned int*)0x60000054
#define DER2                    *(volatile unsigned int*)0x60000074
#define DER3                    *(volatile unsigned int*)0x60000094
#define DFSR0                   *(volatile unsigned int*)0x60000040
#define DFSR1                   *(volatile unsigned int*)0x60000060
#define DTCR0                   *(volatile unsigned int*)0x60000080
#define DTCR1                   *(volatile unsigned int*)0x600000A0

#define EVENT_ENTRY_HIPRI(x)    *(volatile unsigned int*)(0x61008000 + x*4)
#define EVENT_ENTRY_LOWPRI(x)   *(volatile unsigned int*)(0x62008000 + x*4)

#define TRANSFER_ENTRY_HIPRI(num, word)    *(volatile unsigned int*)(0x610080A0 + num*44 + word*4)
#define TRANSFER_ENTRY_LOWPRI(num, word)   *(volatile unsigned int*)(0x620080A0 + num*44 + word*4)

//---------------------------------
//-----FIFO transfers defines
//Event type
#define ETYPE_FIFO_WRITE         0x00000004l
#define ETYPE_FIFO_READ          0x00000005l
// Enable watermark notification
#define EWN                     (0x1l << 16)
//Reload options
#define RLOAD                   (0x1l << 20)
//Transfer Complete Interrupt enable
#define TCINT                   (0x1l << 22)
// Alternate Transfer Complete Interrupt enable
#define ATCINT                  (0x1l << 23)
// Transfer Complete Code
#define TCC(code)               (code << 24)
// Transfer Synchronization
#define SYNC                    (0x1l << 29)
//Quantum Transfer Limit
#define QTSL_MORE_ONE           (0x0l << 30)
#define QTSL_MORE_FOUR          (0x1l << 30)
#define QTSL_MORE_EIGHT         (0x2l << 30)
#define QTSL_MORE_16            (0x3l << 30)
//
//

//---------------------------------
//-------General purpose transfers defines
// Event type
#define ETYPE_GENERAL_PURPOSE    0x00000003
// Element Size
#define ESIZE_8BIT              (0x0l << 16)
#define ESIZE_16BIT             (0x1l << 16)
#define ESIZE_32BIT             (0x2l << 16)
// Counter Configuration
#define CC(count)               (count << 18)



//PTE
#define PTE(numb)               ( (0x28 + numb*0xB) << 8)   





#define CPU_INT_9               0x00020007              
#define CPU_INT_10              0x00030007              
#define CPU_INT_11              0x00040007              
#define CPU_INT_12              0x00050007              
#define CPU_INT_13              0x00060007              
#define CPU_INT_15              0x00070007              

#define WORD_0                  0
#define WORD_1                  1
#define WORD_2                  2
#define WORD_3                  3
#define WORD_4                  4
#define WORD_5                  5
#define WORD_6                  6
#define WORD_7                  7
#define WORD_8                  8
#define WORD_9                  9
#define WORD_10                 10

#define TRANSFER_ENTRY_0        0
#define TRANSFER_ENTRY_1        1
#define TRANSFER_ENTRY_2        2
#define TRANSFER_ENTRY_3        3
#define TRANSFER_ENTRY_4        4
#define TRANSFER_ENTRY_5        5
#define TRANSFER_ENTRY_6        6
#define TRANSFER_ENTRY_7        7

#define EVENT_0                 0x00000001
#define EVENT_1                 0x00000002
#define EVENT_2                 0x00000004
#define EVENT_3                 0x00000008
#define EVENT_4                 0x00000010
#define EVENT_5                 0x00000020
#define EVENT_6                 0x00000040
#define EVENT_7                 0x00000080
#define EVENT_8                 0x00000100
#define EVENT_9                 0x00000200
#define EVENT_10                0x00000400
#define EVENT_11                0x00000800
#define EVENT_12                0x00001000
#define EVENT_13                0x00002000
#define EVENT_14                0x00004000
#define EVENT_15                0x00008000
#define EVENT_16                0x00010000
#define EVENT_17                0x00020000
#define EVENT_18                0x00040000
#define EVENT_19                0x00080000
#define EVENT_20                0x00100000
#define EVENT_21                0x00200000
#define EVENT_22                0x00400000
#define EVENT_23                0x00800000
#define EVENT_24                0x01000000
#define EVENT_26                0x04000000
#define EVENT_27                0x08000000
#define EVENT_28                0x10000000
#define EVENT_29                0x20000000
#define EVENT_30                0x40000000
#define EVENT_31                0x80000000

/*******************************************************************************
SPI Registers
 *******************************************************************************/
#define SPIGCR1_0               *(volatile unsigned int*)0x48000000
#define SPIGCR1_1               *(volatile unsigned int*)0x48000004
#define SPIINT1_0               *(volatile unsigned int*)0x48000008
#define SPILVL1                 *(volatile unsigned int*)0x4800000C
#define SPIFLG1                 *(volatile unsigned int*)0x48000010
#define SPIPC1_0                *(volatile unsigned int*)0x48000014
#define SPIPC1_1                *(volatile unsigned int*)0x48000018
#define SPIPC1_2                *(volatile unsigned int*)0x4800001C
#define SPIPC1_3                *(volatile unsigned int*)0x48000020
#define SPIPC1_4                *(volatile unsigned int*)0x48000024
#define SPIPC1_5                *(volatile unsigned int*)0x48000028
#define SPIDAT1_0               *(volatile unsigned int*)0x48000038
#define SPIDAT1_1               *(volatile unsigned int*)0x4800003C
#define SPIBUF1                 *(volatile unsigned int*)0x48000040
#define SPIEMU1                 *(volatile unsigned int*)0x48000044
#define SPIDELAY1               *(volatile unsigned int*)0x48000048
#define SPIDEF1                 *(volatile unsigned int*)0x4800004C
#define SPIFMT1_0               *(volatile unsigned int*)0x48000050
#define SPIFMT1_1               *(volatile unsigned int*)0x48000054
#define SPIFMT1_2               *(volatile unsigned int*)0x48000058
#define SPIFMT1_3               *(volatile unsigned int*)0x4800005C
#define TGINTVEC1_0             *(volatile unsigned int*)0x48000060
#define TGINTVEC1_1             *(volatile unsigned int*)0x48000064

 

#endif // _TMS320C67PX_H_
