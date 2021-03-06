/*
 * Copyright (C) 2008 Renesas Solutions Corp.
 * Copyright (C) 2008 Nobuhiro Iwamatsu <iwamatsu.nobuhiro@renesas.com>
 * Copyright (C) 2007 Kenati Technologies, Inc.
 *
 * board/sh7763rdp/sh7763rdp.c
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

#include <common.h>
#include <asm/io.h>
#include <asm/processor.h>

#define CPU_CMDREG	0xB1000006
#define PDCR        0xffef0006
#define PECR        0xffef0008
#define PFCR        0xffef000a
#define PGCR        0xffef000c
#define PHCR        0xffef000e
#define PJCR        0xffef0012
#define PKCR        0xffef0014
#define PLCR        0xffef0016
#define PMCR        0xffef0018
#define PSEL1       0xffef0072
#define PSEL2       0xffef0074
#define PSEL3       0xffef0076

int checkboard(void)
{
	puts("BOARD: Renesas SH7763 RDP\n");
	return 0;
}

int board_init(void)
{
	vu_short dat;

	*(vu_short *)(CPU_CMDREG) |= 0x0001;

	/* GPIO Setting (eth1) */
	dat = *(vu_short *)(PSEL1);
	*(vu_short *)PSEL1 = ((dat & ~0xff00) | 0x2400);
	*(vu_short *)PFCR = 0;
	*(vu_short *)PGCR = 0;
	*(vu_short *)PHCR = 0;

	return 0;
}

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_memstart = CFG_SDRAM_BASE;
	gd->bd->bi_memsize = CFG_SDRAM_SIZE;
	printf("DRAM:  %dMB\n", CFG_SDRAM_SIZE / (1024 * 1024));
	return 0;
}

void led_set_state (unsigned short value)
{
}
