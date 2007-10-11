/*
 * (C) Copyright 2006
 * Thomas Waehner, TQ-System GmbH, thomas.waehner@tqs.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.         See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/processor.h>
#include <asm/immap_85xx.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <ioports.h>

#ifdef CONFIG_NAND

#include <nand.h>

DECLARE_GLOBAL_DATA_PTR;

/* index of UPM RAM array run pattern for NAND command cycle */
#define	CFG_NAND_UPM_WRITE_CMD_OFS	0x08

/* index of UPM RAM array run pattern for NAND address cycle */
#define	CFG_NAND_UPM_WRITE_ADDR_OFS	0x10

#define WRITE_NAND(d, adr) do{ *(volatile __u8 *)((unsigned long)(adr)) = (__u8)d; } while(0)
#define READ_NAND(adr) ((volatile unsigned char)(*(volatile __u8 *)(unsigned long)(adr)))

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

/* Structure for table with supported UPM timings */
typedef struct {
	ulong frequency;
	const ulong *pUpmTable;
	uchar gpl4Disable;
	uchar ehtr;
	uchar ead;

} NAND_UPM_FREQ_TABLE_S;

/* NAND-FLASH UPM tables for TQM85XX according to TQM8548.pq.timing.101.doc */

/* UPM pattern for bus clock = 25 MHz */
static const ulong upmTable25[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff32000, 0x0fa32000, 0x3fb32005, 0xfffffc00,
	/* 0x04 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff2c30, 0x00ff2c30, 0x0fff2c35, 0xfffffc00,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3ec30, 0x00f3ec30, 0x0ff3ec35, 0xfffffc00,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f32c00, 0x00f32c00, 0x0ff32c05, 0xfffffc00,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 33.3 MHz */
static const ulong upmTable33[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff32000, 0x0fa32100, 0x3fb32005, 0xfffffc00,
	/* 0x04 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff2c30, 0x00ff2c30, 0x0fff2c35, 0xfffffc00,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3ec30, 0x00f3ec30, 0x0ff3ec35, 0xfffffc00,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f32c00, 0x00f32c00, 0x0ff32c05, 0xfffffc00,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 41.7 MHz */
static const ulong upmTable42[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff32000, 0x0fa32100, 0x3fb32005, 0xfffffc00,
	/* 0x04 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff2c30, 0x00ff2c30, 0x0fff2c35, 0xfffffc00,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3ec30, 0x00f3ec30, 0x0ff3ec35, 0xfffffc00,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f32c00, 0x00f32c00, 0x0ff32c05, 0xfffffc00,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 50 MHz */
static const ulong upmTable50[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff33000, 0x0fa33100, 0x0fa33005, 0xfffffc00,
	/* 0x04 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff3d30, 0x00ff3c30, 0x0fff3c35, 0xfffffc00,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3fd30, 0x00f3fc30, 0x0ff3fc35, 0xfffffc00,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f33d00, 0x00f33c00, 0x0ff33c05, 0xfffffc00,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 66.7 MHz */
static const ulong upmTable67[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff33000, 0x0fe33000, 0x0fa33100, 0x0fa33000,
	/* 0x04 */ 0x0fa33005, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff3d30, 0x00ff3c30, 0x0fff3c30, 0x0fff3c35,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3fd30, 0x00f3fc30, 0x0ff3fc30, 0x0ff3fc35,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f33d00, 0x00f33c00, 0x0ff33c00, 0x0ff33c05,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 83.3 MHz */
static const ulong upmTable83[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff33000, 0x0fe33000, 0x0fa33100, 0x0fa33000,
	/* 0x04 */ 0x0fa33005, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff3e30, 0x00ff3c30, 0x0fff3c30, 0x0fff3c35,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3fe30, 0x00f3fc30, 0x0ff3fc30, 0x0ff3fc35,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f33e00, 0x00f33c00, 0x0ff33c00, 0x0ff33c05,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 100 MHz */
static const ulong upmTable100[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff33100, 0x0fe33000, 0x0fa33200, 0x0fa33000,
	/* 0x04 */ 0x0fa33005, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff3f30, 0x00ff3c30, 0x0fff3c30, 0x0fff3c35,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3ff30, 0x00f3fc30, 0x0ff3fc30, 0x0ff3fc35,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f33f00, 0x00f33c00, 0x0ff33c00, 0x0ff33c05,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 133.3 MHz */
static const ulong upmTable133[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff33100, 0x0fe33000, 0x0fa33300, 0x0fa33000,
	/* 0x04 */ 0x0fa33000, 0x0fa33005, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff3f30, 0x00ff3d30, 0x0fff3d30, 0x0fff3c35,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3ff30, 0x00f3fd30, 0x0ff3fd30, 0x0ff3fc35,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f33f00, 0x00f33d00, 0x0ff33d00, 0x0ff33c05,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

/* UPM pattern for bus clock = 166.7 MHz */
static const ulong upmTable167[] = {
	/* Offset *//* UPM Read Single RAM array entry -> NAND Read Data */
	/* 0x00 */ 0x0ff33200, 0x0fe33000, 0x0fa33300, 0x0fa33300,
	/* 0x04 */ 0x0fa33005, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write CMD */
	/* 0x08 */ 0x00ff3f30, 0x00ff3f30, 0x0fff3e30, 0xffff3c35,
	/* 0x0C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Read Burst RAM array entry -> NAND Write ADDR */
	/* 0x10 */ 0x00f3ff30, 0x00f3ff30, 0x0ff3fe30, 0x0ff3fc35,
	/* 0x14 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Single RAM array entry -> NAND Write Data */
	/* 0x18 */ 0x00f33f00, 0x00f33f00, 0x0ff33e00, 0x0ff33c05,
	/* 0x1C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,

	/* UPM Write Burst RAM array entry -> unused */
	/* 0x20 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x24 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x28 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x2C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Refresh Timer RAM array entry -> unused */
	/* 0x30 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x34 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc00,
	/* 0x38 */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,

	/* UPM Exception RAM array entry -> unsused */
	/* 0x3C */ 0xfffffc00, 0xfffffc00, 0xfffffc00, 0xfffffc01,
};

static int chipsel = 0;

/* Supported UPM timings */
NAND_UPM_FREQ_TABLE_S nandUpmFreqTable[] = {
	/* nominal freq. | ptr to table | GPL4 dis. | EHTR  | EAD */
	{25000000, upmTable25, TRUE, FALSE, FALSE},
	{33333333, upmTable33, TRUE, FALSE, FALSE},
	{41666666, upmTable42, TRUE, FALSE, FALSE},
	{50000000, upmTable50, FALSE, FALSE, FALSE},
	{66666666, upmTable67, FALSE, FALSE, FALSE},
	{83333333, upmTable83, FALSE, FALSE, FALSE},
	{100000000, upmTable100, FALSE, TRUE, TRUE},
	{133333333, upmTable133, FALSE, TRUE, TRUE},
	{166666666, upmTable167, FALSE, TRUE, TRUE},
};

#define N_NAND_UPM_FREQS (sizeof(nandUpmFreqTable)/sizeof(NAND_UPM_FREQ_TABLE_S))

/* nand flash base address */
static u8 hwctl = 0;

/*
 * write into UPMB ram
 */
static void upmb_write (u_char addr, ulong val)
{
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile ccsr_lbc_t *lbc = &immap->im_lbc;

	lbc->mdr = val;
	asm ("sync; isync; msync");

	lbc->mbmr = (lbc->mbmr & ~(MxMR_OP_NORM | MxMR_MAD)) |
	    MxMR_OP_WARR | (addr & MxMR_MAD);
	asm ("sync; isync; msync");

	/* dummy access to perform write */
	*(volatile u_char *)CFG_NAND0_BASE = 0;
	asm ("sync; isync; msync");

	lbc->mbmr = (lbc->mbmr & ~MxMR_OP_WARR) | MxMR_OP_NORM;
	asm ("sync; isync; msync");
}

/*
 * Initialize UPM for NAND flash access.
 */
void nand_upm_init (void)
{
	uint i;
	uint or3 = CFG_OR3_PRELIM;
	uint clock = get_local_bus_freq (0);
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile ccsr_lbc_t *lbc = &immap->im_lbc;
	volatile const ulong *pUpmTable = NULL;

	lbc->br3 = 0;		/* disable bank and reset all bits */

	lbc->br3 = CFG_BR3_PRELIM;

	/*
	 * Search appropriate UPM table for bus clock.
	 * If the bus clock exceeds a tolerated value, take the UPM timing for
	 * the next higher supported frequency to ensure that access works
	 * (even the access may be slower then).
	 */
	for (i = 0;
	     (i < N_NAND_UPM_FREQS) && (clock > nandUpmFreqTable[i].frequency);
	     i++) ;

	if (i >= N_NAND_UPM_FREQS) {
		/* no valid entry found */
		/* take last entry with configuration for max. bus clock */
		i--;
	}

	if (nandUpmFreqTable[i].ehtr == TRUE) {
		/* EHTR must be set due to TQM8548 timing specification */
		or3 |= ORxU_EHTR;
	}
	if (nandUpmFreqTable[i].ead == TRUE) {
		/* EAD must be set due to TQM8548 timing specification */
		or3 |= ORxU_EAD;
	}

	lbc->or3 = or3;

	/* Assign address of table */
	pUpmTable = nandUpmFreqTable[i].pUpmTable;

	for (i = 0; i < 64; i++) {
		upmb_write (i, *pUpmTable);
		pUpmTable++;
	}

	/* Put UPM back to normal operation mode */
	if (nandUpmFreqTable[i].gpl4Disable == TRUE) {
		/* GPL4 must be disabled according to timing specification */
		lbc->mbmr = MxMR_OP_NORM | MxMR_GPL4_DIS;
	}

	return;
}				/* nand_upm_init */

/*
 * nand_upm_cmd - sends a command to NAND flash
 *
 * This function sends a command to the NAND flash using a UPM run pattern
 * which sets CLE.
 */
void nand_upm_cmd (struct mtd_info *mtd, unsigned char cmd)
{
	struct nand_chip *this = mtd->priv;
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile ccsr_lbc_t *lbc = &immap->im_lbc;
	int i;

	/* Put UPM to run pattern mode for NAND command write */
	lbc->mbmr = (lbc->mbmr & ~(MxMR_OP_MSK | MxMR_MAD)) |
	    MxMR_OP_RUN | CFG_NAND_UPM_WRITE_CMD_OFS;
	asm ("sync; isync; msync");

	/* Just output address on AD[0..7] (=MSB) */
	lbc->mar = cmd << 24;
	asm ("sync; isync; msync");

	/* UPM dummy write */
	writeb (0, this->IO_ADDR_W + chipsel * CFG_NAND_CS_DIST);
	asm ("sync; isync; msync");

	/* give upm time to run pattern */
	for (i = 0; i < 300; i++) {
		asm ("nop");
	}

	/* Put UPM back to normal operation mode */
	lbc->mbmr = (lbc->mbmr & ~MxMR_OP_MSK) | MxMR_OP_NORM;
	asm ("sync; isync; msync");
}				/* nand_upm_cmd */

/*
 * nand_upm_addr - sends an address to NAND flash
 *
 * This function sends an address to the NAND flash using a UPM run pattern
 * which sets ALE.
 */
void nand_upm_addr (struct mtd_info *mtd, unsigned char adr)
{
	struct nand_chip *this = mtd->priv;
	volatile immap_t *immap = (immap_t *) CFG_IMMR;
	volatile ccsr_lbc_t *lbc = &immap->im_lbc;
	int i;

	/* Put UPM to run pattern mode for NAND address write */
	lbc->mbmr = (lbc->mbmr & ~(MxMR_OP_MSK | MxMR_MAD)) |
	    MxMR_OP_RUN | CFG_NAND_UPM_WRITE_ADDR_OFS;
	asm ("sync; isync; msync");

	/* Just output address on AD[0..7] (=MSB) */
	lbc->mar = adr << 24;
	asm ("sync; isync; msync");

	/* UPM dummy write */
	writeb (0, this->IO_ADDR_W + chipsel * CFG_NAND_CS_DIST);
	asm ("sync; isync; msync");

	/* give upm time to run pattern */
	for (i = 0; i < 300; i++) {
		asm ("nop");
	}

	/* Put UPM back to normal operation mode */
	lbc->mbmr = (lbc->mbmr & ~MxMR_OP_MSK) | MxMR_OP_NORM;
	asm ("sync; isync; msync");
}				/* nand_upm_adr */

static void upmnand_hwcontrol (struct mtd_info *mtdinfo, int cmd)
{
	switch (cmd) {
	case NAND_CTL_SETCLE:
		hwctl |= 0x1;
		break;
	case NAND_CTL_CLRCLE:
		hwctl &= ~0x1;
		break;

	case NAND_CTL_SETALE:
		hwctl |= 0x2;
		break;

	case NAND_CTL_CLRALE:
		hwctl &= ~0x2;
		break;
	}
}

static void upmnand_write_byte (struct mtd_info *mtdinfo, u_char byte)
{
	struct nand_chip *this = mtdinfo->priv;
	ulong base = (ulong) (this->IO_ADDR_W + chipsel * CFG_NAND_CS_DIST);

	if (hwctl & 0x1) {
		nand_upm_cmd (mtdinfo, byte);
	} else if (hwctl & 0x2) {
		nand_upm_addr (mtdinfo, byte);
	} else {
		WRITE_NAND (byte, base);
	}
}

static u_char upmnand_read_byte (struct mtd_info *mtdinfo)
{
	struct nand_chip *this = mtdinfo->priv;
	ulong base = (ulong) (this->IO_ADDR_W + chipsel * CFG_NAND_CS_DIST);

	return READ_NAND (base);
}

#ifdef CONFIG_HAVE_NAND_RB
static int tqm_dev_ready (struct mtd_info *mtdinfo)
{
	/** FIXME query GPIO PIN where NAND R/B is attached to */

	/* constant delay (see also tR in the datasheet) */
	udelay (NAND_BIG_DELAY_US);
	return 1;
}
#endif				/* CONFIG_HAVE_NAND_RB */

#ifndef CONFIG_NAND_SPL
static void tqm_read_buf (struct mtd_info *mtdinfo, uint8_t * buf, int len)
{
	struct nand_chip *this = mtdinfo->priv;
	unsigned char *base =
	    (unsigned char *)(this->IO_ADDR_W + chipsel * CFG_NAND_CS_DIST);
	int i;

	for (i = 0; i < len; i++)
		buf[i] = *base;
}

static void tqm_write_buf (struct mtd_info *mtdinfo, const uint8_t * buf,
			   int len)
{
	struct nand_chip *this = mtdinfo->priv;
	unsigned char *base =
	    (unsigned char *)(this->IO_ADDR_W + chipsel * CFG_NAND_CS_DIST);
	int i;

	for (i = 0; i < len; i++)
		*base = buf[i];
}

static int tqm_verify_buf (struct mtd_info *mtdinfo, const uint8_t * buf,
			   int len)
{
	struct nand_chip *this = mtdinfo->priv;
	unsigned char *base =
	    (unsigned char *)(this->IO_ADDR_W + chipsel * CFG_NAND_CS_DIST);
	int i;

	for (i = 0; i < len; i++)
		if (buf[i] != *base)
			return -1;
	return 0;
}
#endif				/* #ifndef CONFIG_NAND_SPL */

void board_nand_select_device (struct nand_chip *nand, int chip)
{
	chipsel = chip;
}

int board_nand_init (struct nand_chip *nand)
{
	extern void nand_upm_init (void);

	/* initialize LBC3 for nand flash */
	nand_upm_init ();

	nand->eccmode = NAND_ECC_SOFT;

	nand->hwcontrol = upmnand_hwcontrol;
	nand->read_byte = upmnand_read_byte;
	nand->write_byte = upmnand_write_byte;
	nand->chip_delay = NAND_BIG_DELAY_US;
#ifdef CONFIG_HAVE_NAND_RB
	nand->dev_ready = tqm_dev_ready;
#endif				/* CONFIG_HAVE_NAND_RB */

#ifndef CONFIG_NAND_SPL
	nand->write_buf = tqm_write_buf;
	nand->read_buf = tqm_read_buf;
	nand->verify_buf = tqm_verify_buf;
#endif

	board_nand_select_device (nand, 0);
	return 0;
}
#endif				/* CONFIG_NAND */
