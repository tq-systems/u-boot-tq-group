/*
 * Copyright (C) 2008, Guennadi Liakhovetski <lg@denx.de>
 * Copyright (C) 2008, Jens Gehrlein <sew_s@tqs.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm/arch/mx31-regs.h>
#include <config.h>

.macro REG reg, val
	ldr r2, =\reg
	ldr r3, =\val
	str r3, [r2]
.endm

.macro REG8 reg, val
	ldr r2, =\reg
	ldr r3, =\val
	strb r3, [r2]
.endm

.macro DELAY loops
	ldr r2, =\loops
1:
	subs	r2, r2, #1
	nop
	bcs 1b
.endm


/*
 * AIPS setup - Only setup MPROTx registers.
 * The PACR default values are good.
 */
.macro init_aips

	/* Setup the Peripheral Port Remap register inside the core */
	ldr r0, =0x40000015	/* start from AIPS 2GB region */
	mcr p15, 0, r0, c15, c2, 4

	/*
	 * Set all MPROTx to be non-bufferable, trusted for R/W,
	 * not forced to user-mode.
	 */
	ldr r0, =0x43F00000
	ldr r1, =0x77777777
	str r1, [r0, #0x00]
	str r1, [r0, #0x04]
	ldr r0, =0x53F00000
	str r1, [r0, #0x00]
	str r1, [r0, #0x04]

	/*
	 * Clear the on and off peripheral modules Supervisor Protect bit
	 * for SDMA to access them. Did not change the AIPS control registers
	 * (offset 0x20) access type
	 */
	ldr r0, =0x43F00000
	ldr r1, =0x0
	str r1, [r0, #0x40]
	str r1, [r0, #0x44]
	str r1, [r0, #0x48]
	str r1, [r0, #0x4C]
	ldr r1, [r0, #0x50]
	and r1, r1, #0x00FFFFFF
	str r1, [r0, #0x50]

	ldr r0, =0x53F00000
	ldr r1, =0x0
	str r1, [r0, #0x40]
	str r1, [r0, #0x44]
	str r1, [r0, #0x48]
	str r1, [r0, #0x4C]
	ldr r1, [r0, #0x50]
	and r1, r1, #0x00FFFFFF
	str r1, [r0, #0x50]
.endm /* init_aips */


/* RedBoot: MAX (Multi-Layer AHB Crossbar Switch) setup */
.macro init_max
	ldr r0, =0x43F04000
	/* MPR - priority is M4 > M2 > M3 > M5 > M0 > M1 */
	ldr r1, =0x00302154
	str r1, [r0, #0x000]	/* for S0 */
	str r1, [r0, #0x100]	/* for S1 */
	str r1, [r0, #0x200]	/* for S2 */
	str r1, [r0, #0x300]	/* for S3 */
	str r1, [r0, #0x400]	/* for S4 */
	/* SGPCR - always park on last master */
	ldr r1, =0x10
	str r1, [r0, #0x010]	/* for S0 */
	str r1, [r0, #0x110]	/* for S1 */
	str r1, [r0, #0x210]	/* for S2 */
	str r1, [r0, #0x310]	/* for S3 */
	str r1, [r0, #0x410]	/* for S4 */
	/* MGPCR - restore default values */
	ldr r1, =0x0
	str r1, [r0, #0x800]	/* for M0 */
	str r1, [r0, #0x900]	/* for M1 */
	str r1, [r0, #0xA00]	/* for M2 */
	str r1, [r0, #0xB00]	/* for M3 */
	str r1, [r0, #0xC00]	/* for M4 */
	str r1, [r0, #0xD00]	/* for M5 */
.endm /* init_max */


/* RedBoot: M3IF setup */
.macro init_m3if
	/* Configure M3IF registers */
	ldr r1, =0xB8003000
	/*
	* M3IF Control Register (M3IFCTL)
	* MRRP[0] = L2CC0 not on priority list (0 << 0)	= 0x00000000
	* MRRP[1] = L2CC1 not on priority list (0 << 0)	= 0x00000000
	* MRRP[2] = MBX not on priority list (0 << 0)	= 0x00000000
	* MRRP[3] = MAX1 not on priority list (0 << 0)	= 0x00000000
	* MRRP[4] = SDMA not on priority list (0 << 0)	= 0x00000000
	* MRRP[5] = MPEG4 not on priority list (0 << 0)	= 0x00000000
	* MRRP[6] = IPU1 on priority list (1 << 6)	= 0x00000040
	* MRRP[7] = IPU2 not on priority list (0 << 0)	= 0x00000000
	*						------------
	*						  0x00000040
	*/
	ldr r0, =0x00000040
	str r0, [r1]	/* M3IF control reg */
.endm /* init_m3if */


/* RedBoot: To support 133MHz DDR */
.macro init_drive_strength
	/*
	 * Disable maximum drive strength SDRAM/DDR lines by clearing DSE1 bits
	 * in SW_PAD_CTL registers
	 */

	/* SDCLK */
	ldr r1, =0x43FAC200
	ldr r0, [r1, #0x6C]
	bic r0, r0, #(1 << 12)
	str r0, [r1, #0x6C]

	/* CAS */
	ldr r0, [r1, #0x70]
	bic r0, r0, #(1 << 22)
	str r0, [r1, #0x70]

	/* RAS */
	ldr r0, [r1, #0x74]
	bic r0, r0, #(1 << 2)
	str r0, [r1, #0x74]

	/* CS2 (CSD0) */
	ldr r0, [r1, #0x7C]
	bic r0, r0, #(1 << 22)
	str r0, [r1, #0x7C]

	/* DQM3 */
	ldr r0, [r1, #0x84]
	bic r0, r0, #(1 << 22)
	str r0, [r1, #0x84]

	/* DQM2, DQM1, DQM0, SD31-SD0, A25-A0, MA10 (0x288..0x2DC) */
	ldr r2, =22	/* (0x2E0 - 0x288) / 4 = 22 */
pad_loop:
	ldr r0, [r1, #0x88]
	bic r0, r0, #(1 << 22)
	bic r0, r0, #(1 << 12)
	bic r0, r0, #(1 << 2)
	str r0, [r1, #0x88]
	add r1, r1, #4
	subs r2, r2, #0x1
	bne pad_loop
.endm /* init_drive_strength */


/* initialize PLL */
.macro init_pll
	/*
	 * Switch on Display Interface because of the change of the clock
	 * divider. According to the description of PDR0[HSP_PODF] in the
	 * processor reference manual the CCM waits for an ackn. of the IPU.
	 */
	REG	IPU_CONF, IPU_CONF_DI_EN

	/* Clock Control Module: */
	REG	CCM_CCMR, 0x074B0BF5				/* Use CKIH, MCU PLL off */
	DELAY	0x40000
	REG	CCM_CCMR, 0x074B0BF5 | CCMR_MPE			/* MCU PLL on */
	REG	CCM_CCMR, (0x074B0BF5 | CCMR_MPE) & ~CCMR_MDS	/* Switch to MCU PLL */

#if defined CONFIG_MX31_CPU_399_MHZ
	/* Platform clocks */
	REG	CCM_PDR0, PDR0_CSI_PODF(0x1ff)	/* camera sensor interface */		\
			| PDR0_PER_PODF(6-1)	/* periph. if derived from USB clock */	\
			| PDR0_HSP_PODF(3-1)	/* IPU 133 MHz */			\
			| PDR0_NFC_PODF(7-1)	/* NAND FC 19 MHz */			\
			| PDR0_IPG_PODF(2-1)	/* peripherals 66.5 MHz */		\
			| PDR0_MAX_PODF(3-1)	/* AHB 133 MHz */			\
			| PDR0_MCU_PODF(1-1)	/* core 399 MHz */
	/* MCU PLL */
	REG	CCM_MPCTL, PLL_PD(1-1) | PLL_MFD(52-1) | PLL_MFI(7) | PLL_MFN(35)
#elif defined CONFIG_MX31_CPU_532_MHZ
	/* Platform clocks */
	REG	CCM_PDR0, PDR0_CSI_PODF(0x1ff)	/* camera sensor interface */		\
			| PDR0_PER_PODF(8-1)	/* periph. if derived from USB clock */	\
			| PDR0_HSP_PODF(4-1)	/* IPU 133 MHz */			\
			| PDR0_NFC_PODF(7-1)	/* NAND FC 19 MHz */			\
			| PDR0_IPG_PODF(2-1)	/* peripherals 66.5 MHz */		\
			| PDR0_MAX_PODF(4-1)	/* AHB 133 MHz */			\
			| PDR0_MCU_PODF(1-1)	/* core 532 MHz */
	/* MCU PLL */
	REG	CCM_MPCTL, PLL_PD(1-1) | PLL_MFD(52-1) | PLL_MFI(10) | PLL_MFN(12)
#else
#error Please define CPU core frequency (CONFIG_CPU_399_MHZ or CONFIG_CPU_532_MHZ)!
#endif /* CONFIG_CPU_xxx_MHZ */

	/* Serial PLL. Default: 1, 4, 12, 1 */
	REG	CCM_SPCTL, PLL_PD(2-1) | PLL_MFD(5-1) | PLL_MFI(12) | PLL_MFN(1)
.endm /* init_pll */


/* setup SDRAM on CSD0 */
.macro init_sdram
	/*
	 * Enhanced SDRAM Miscellaneous Register
	 *	Latency Hiding Enable
	 *	LPDDR delay line measure unit is enabled
	 *	LPDDR Delay Line is not reset
	 *	Enable Mobile DDR SDRAM operation
	 *	Soft Reset is disabled
	 */
	REG	ESDMISC, 0x00000004

	/*
	 * Enhanced SDRAM Configuration Register 0
	 *	tXP 4 clock delay before new COMMAND issued to LPDDR
	 *	tWTR tLPDDR WRITE to READ Command Delay = 2 clock
	 *	tRP SDRAM Row Precharge Delay = 3 clock
	 *	tMRD SDRAM Load Mode Register to ACTIVE Command = 3 clock
	 *	tWR SDRAM WRITE to PRECHARGE Command = 3 clock
	 *	tRAS SDRAM ACTIVE to PRECHARGE Command = 6 clock
	 *	tRRD ACTIVE Bank A to ACTIVE Bank B Command = 2 clock
	 *	tCAS CAS Latency = 3 clock
	 *	tRCD SDRAM Row to Column Delay = 3 clock
	 *	tRC SDRAM Row Cycle Delay = 13 clocks
	 */
	REG	ESDCFG0, 0x006ac73c

	/*
	 * Enhanced SDRAM Control Register 0
	 *	Enhanced SDRAM Controller Enable = 1
	 *	SDRAM Controller Operating Mode = 001 Precharge Command
	 *	Supervisor Protect = 0 User mode accesses are allowed to this
	 *		chip select region
	 *	ROW Row Address Width. = 011 14 Row Addresses
	 *	COL Column Address Width = 01 9 Col Addresses
	 *	DSIZ SDRAM Memory Data Width = 00 00 16-bit memory width
	 *		aligned to D[31:16]
	 *	SREFR SDRAM Refresh Rate = 000 Refresh Disabled
	 *		(bit field reset value)
	 *	PWDT Power Down = 00 Disabled (bit field reset value) Run Mode
	 *	FP Full Page = 0 0 Burst Length of the external memory device
	 *		is not set to Full Page
	 *	BL Burst Length = 0
	 *	PRCT Precharge Timer 000000 Disabled (Bit field reset value)
	 */
	REG	ESDCTL0, 0x92200000
	REG	0x80000f00, 0x12344321


	/*
	 * Enhanced SDRAM Control Register 0
	 *	Enhanced SDRAM Controller Enable = 1
	 *	SDRAM Controller Operating Mode = 010 Auto-Refresh Command
	 *	Supervisor Protect = 0 User mode accesses are allowed to this
	 *		chip select region
	 *	ROW Row Address Width. = 011 14 Row Addresses
	 *	COL Column Address Width = 01 9 Col Addresses
	 *	DSIZ SDRAM Memory Data Width = 00 00 16-bit memory width
	 *		aligned to D[31:16]
	 *	SREFR SDRAM Refresh Rate = 000 Refresh Disabled
	 *		(bit field reset value)
	 *	PWDT Power Down = 00 Disabled (bit field reset value) Run Mode
	 *	FP Full Page = 0 0 Burst Length of the external memory device
	 *		is not set to Full Page
	 *	BL Burst Length = 0
	 *	PRCT Precharge Timer 000000 Disabled (Bit field reset value)
	 */
	REG	ESDCTL0, 0xa2200000
	REG	0x80000000, 0x12344321
	REG	0x80000000, 0x12344321

	/*
	 * Enhanced SDRAM Control Register 0
	 *	Enhanced SDRAM Controller Enable = 1
	 *	SDRAM Controller Operating Mode = 001 Precharge Command
	 *	Supervisor Protect = 0 User mode accesses are allowed to this
	 *		chip select region
	 *	ROW Row Address Width. = 011 14 Row Addresses
	 *	COL Column Address Width = 01 9 Col Addresses
	 *	DSIZ SDRAM Memory Data Width = 00 00 16-bit memory width
	 *		aligned to D[31:16]
	 *	SREFR SDRAM Refresh Rate = 000 Refresh Disabled
	 *		(bit field reset value)
	 *	PWDT Power Down = 00 Disabled (bit field reset value) Run Mode
	 *	FP Full Page = 0 0 Burst Length of the external memory device
	 *		is not set to Full Page.
	 *	BL Burst Length = 0
	 *	PRCT Precharge Timer 000000 Disabled (Bit field reset value)
	 */
	REG	ESDCTL0, 0xb2200000
	REG8	0x80000033, 0xda
	REG8	0x81000000, 0xff

	/*
	 * Enhanced SDRAM Control Register 0
	 *	Enhanced SDRAM Controller Enable = 1
	 *	SDRAM Controller Operating Mode = 000 Normal Read/Write
	 *	Supervisor Protect = 0 User mode accesses are allowed to this
	 *		chip select region
	 *	ROW Row Address Width. = 011 14 Row Addresses
	 *	COL Column Address Width = 10 10 Col Addresses
	 *	DSIZ SDRAM Memory Data Width = 10 32-bit memory width
	 *	SREFR SDRAM Refresh Rate = 011 Refresh 7,81 microseconds
	 *	PWDT Power Down = 00 Disabled (bit field reset value) Run Mode
	 *	FP Full Page = 0 0 Burst Length of the external memory device
	 *		is not set to Full Page
	 *	BL Burst Length = 8
	 *	PRCT Precharge Timer 000000 Disabled (Bit field reset value)
	 */
	REG	ESDCTL0, 0x83226080
	REG	0x80000000, 0xDEADBEEF

	/*
	 * Enhanced SDRAM Miscellaneous Register
	 *	Latency Hiding Enable
	 *	LPDDR delay line measure unit is enabled
	 *	LPDDR Delay Line is reset
	 *	Enable Mobile DDR SDRAM operation
	 *	Soft Reset is disabled
	 */
	REG	ESDMISC, 0x0000000c
.endm /* init_sdram */


.globl lowlevel_init
lowlevel_init:
	init_aips

	init_max

	init_m3if

	init_drive_strength

	init_pll

	init_sdram

	mov pc, lr
