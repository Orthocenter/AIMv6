/*
 * Copyright (C) 2015 David Gao <davidgao1001@gmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <config.h>

#ifdef GTC_A9MPCORE

#include <sys/types.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <drivers/clock/gtc-a9mpcore.h>

void enable_gtc_interrupt() {
	u32 r;

	// Distributor:

	// enables the forwarding of GTC interrupts
	r = in32(icd_base + ICD_ISER_OFFSET);
	r |= 1 << 27;
	out32(icd_base + ICD_ISER_OFFSET, r);

	// set priority of GTC interrupts
	r = in32(icd_base + ICD_IPR_OFFSET + 4 * 6); // 4 * (27 div 4)
	r &= 0x00FFFFFF;
	r |= 0x80000000; // priority: 0x80
	out32(icd_base + ICD_IPR_OFFSET + 4 * 6, r);

	// set target of GTC interrupts
	r = in32(icd_base + ICD_IPTR_OFFSET + 4 * 6);
	r &= 0x00FFFFFF;
	r |= 0xFF000000;
	out32(icd_base + ICD_IPTR_OFFSET + 4 * 6, r);

	// GTC:

	// IRQ enable
	r = in32(GTC_BASE + GTC_CTRL_OFFSET);
	r |= 1 << 2;
	out32(GTC_BASE + GTC_CTRL_OFFSET, r);
}

void set_gtc_comparator(u64 t) {
	u32 r;

	// disable Comp
	r = in32(GTC_BASE + GTC_CTRL_OFFSET);
	r &= ~(1 << 1);
	out32(GTC_BASE + GTC_CTRL_OFFSET, r);

	// lower bits
	out32(GTC_BASE + GTC_COMPARATOR_LO_OFFSET, (u32)(t & 0xFFFFFFFF));
	// higher bits
	out32(GTC_BASE + GTC_COMPARATOR_HI_OFFSET, (u32)(t >> 32));

	// enable Comp
	r = in32(GTC_BASE + GTC_CTRL_OFFSET);
	r |= 1 << 1;
	out32(GTC_BASE + GTC_CTRL_OFFSET, r);
}

u64 gtc_get_time()
{
	u64 time;
	u32 hi, lo, tmp;
	/* HI-LO-HI reading because GTC is 64bit */
	do {
		hi = in32(GTC_BASE + GTC_COUNTER_HI_OFFSET);
		lo = in32(GTC_BASE + GTC_COUNTER_LO_OFFSET);
		tmp = in32(GTC_BASE + GTC_COUNTER_HI_OFFSET);
	} while (hi != tmp);
	time = (u64)hi << 32;
	time |= lo;
	return time;
}

void sleep(int sec)
{
	u64 time, time1;
	time = gtc_get_time();
	time += GTC_TPS * sec;
	do {
		time1 = gtc_get_time();
	} while (time1 < time);
}

void usleep(int usec)
{
	u64 time, time1;
	time = gtc_get_time();
	time += GTC_TPUS * usec;
	do {
		time1 = gtc_get_time();
	} while (time1 < time);
}

#endif /* GTC_A9MPCORE */
