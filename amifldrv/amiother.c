//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amiother.c
 *
 */
#include <linux/fs.h>

#include "amifldrv.h"

#define DEFAULT_SMI_PORT	0xB2

unsigned short SwSmiPort = 0;

static const char acpi_fadt_path[] = "/sys/firmware/acpi/tables/FACP";

static struct tagAMIDRV_INFO amidrv_info =
{
	0x01,					// Struct Version
	sizeof(AMIDRV_INFO),	// Struct Length
	2,						// Driver Version Major
	0						// Driver Version Minor
};

int amidrv_getinfo(void *arg)
{
	unsigned long	result;
	AMIDRV_INFO		arg_drvinfo;

	// check input
	if (!arg) return -EINVAL;

	// we only need the first and second members to make sure the input buffer is valid
	result = copy_from_user((void *)&arg_drvinfo, arg, sizeof(unsigned int) * 2);
	if (arg_drvinfo.InfoLength < amidrv_info.InfoLength) return -EINVAL;

	// copy the driver info to user space
	result = copy_to_user(arg, (void *)&amidrv_info, amidrv_info.InfoLength);

	return 0;
}

unsigned short get_smi_port(void)
{
	struct file *fadt;
	unsigned char fadt_data[52];	// we only need SMI_CMD, which is offset 0x30
	loff_t pos = 0;

	memset(fadt_data, 0, sizeof(fadt_data));
	fadt = filp_open(acpi_fadt_path, O_RDONLY, 0);
	if (IS_ERR(fadt)) return DEFAULT_SMI_PORT;
	kernel_read(fadt, (void *)fadt_data, sizeof(fadt_data), &pos);
	filp_close(fadt, 0);

	return *(unsigned short *)&fadt_data[48];
}

int io_port_filter(unsigned short Port)
{
	unsigned short	i, ioPortAllow[] = {
					0xB2,			// Standard SW-SMI
					0xCA0,0xCA4,	// KCS Port 1
					0xCA8,0xCAC,	// KCS Port 2
					0xCA2,0xCA3		// KCS Port 3
					};

	for (i = 0; i < sizeof(ioPortAllow) / sizeof(unsigned short); i++)
		if (Port == ioPortAllow[i])
			return 1;
	
	if(SwSmiPort == Port)
		return 1;
	
	return 0;
}