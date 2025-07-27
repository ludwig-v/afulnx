//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amifldrv.h
 *
 */

#ifndef _AMIFLDRV__H_
#define _AMIFLDRV__H_

#ifdef BUILD_AMIFLDRV_MOD
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/vmalloc.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#include <linux/wrapper.h>
#endif

/*
 * 1. Macro definitions
 *
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	#define mem_map_reserve(p)		set_bit(PG_reserved, &((p)->flags))
	#define mem_map_unreserve(p)	clear_bit(PG_reserved, &((p)->flags))
	MODULE_AUTHOR("American Megatrends Inc.");
	MODULE_DESCRIPTION("AMI Generic Utility Driver");
	MODULE_LICENSE("Proprietary");
#endif
#define print_msg(severity, fmt, args...) \
			printk(severity "%s: " fmt "\n", THIS_MODULE->name, ##args)
#define print_warn(args...) print_msg(KERN_WARNING, args)
#define print_info(args...) print_msg(KERN_INFO, args)
#endif	// BUILD_AMIFLDRV_MOD

#define AMIFLDRV_NAME		"amifldrv"
#define MAX_PHY_MEM_SIZE	(KMALLOC_ARRAY_SIZE * 1024)
#define KMALLOC_ARRAY_SIZE	128			/// array of physical memory buffers allocated
#define AFU_ATTRIBUTE_FUNC

/*
 * 2. Data structures
 *
 */
#include "amifldrvdefs.h"

/*
 * 3. Function prototypes
 *
 */
/*
 * amimemdrv.c
 */
int amimemdrv_init(void);
void amimemdrv_release(void);

/*
 * amiiodrv.c
 */
int amiio_write8(void *arg);
int amiio_write16(void *arg);
int amiio_write32(void *arg);
int amiio_read8(void *arg);
int amiio_read16(void *arg);
int amiio_read32(void *arg);

/*
 * amismmbufferv1.c
 * amismmbufferv2.c
 */
int amismmbufferv1_init(void);
int amismmbufferv1_filter(unsigned long start, unsigned long length);
int amismmbufferv2_init(void);
int amismmbufferv2_filter(unsigned long start, unsigned long length);

/*
 * 4. Others
 *
 */
int amidrv_getinfo(void *arg);
unsigned short get_smi_port(void);
int io_port_filter(unsigned short Port);

#endif	// _AMIFLDRV__H_
