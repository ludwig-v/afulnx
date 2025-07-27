//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amimemdrv.c
 *
 */

#include "amifldrv.h"

static AMIFLDRV_ALLOC	physical_memory_alloc_record[KMALLOC_ARRAY_SIZE];
static unsigned long	physical_memory_alloc_count = 0, physical_memory_length;
static void				*physical_memory_virtual_addr, *physical_memory_addr;

int amimemdrv_init(void)
{
	// initialize local data
	memset(physical_memory_alloc_record, 0, sizeof(AMIFLDRV_ALLOC) * KMALLOC_ARRAY_SIZE);
	return 0;
}

void amimemdrv_release(void)
{
	unsigned long	i;
	unsigned char	*virt_addr;

	if (physical_memory_alloc_count > 0) {
		for (i = 0; i < physical_memory_alloc_count; i++) {
			physical_memory_addr			= physical_memory_alloc_record[i].kmallocptr;
			physical_memory_virtual_addr	= physical_memory_alloc_record[i].kvirtadd;
			physical_memory_length			= physical_memory_alloc_record[i].kvirtlen;
			if (physical_memory_addr) {
				for (virt_addr = (unsigned char*)physical_memory_virtual_addr;
					 virt_addr < (unsigned char*)physical_memory_virtual_addr + physical_memory_length;
					 virt_addr += PAGE_SIZE)
					mem_map_unreserve(virt_to_page(virt_addr));

				kfree(physical_memory_addr);
			}
		}
	}
	physical_memory_alloc_count = 0;
}

