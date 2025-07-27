//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amismmbufferv1.c
 *
 */

#include "amifldrv.h"

static unsigned long smm_mailbox = 0;
static unsigned long smm_buffer = 0;

extern int int_write8(AMIFLDRV_PORTRW *);
extern unsigned short SwSmiPort;

int amismmbufferv1_init(void)
{
	unsigned short smi_port;
	SMI_MAILBOX *mailbox;
	AMIFLDRV_PORTRW io_context;

	smi_port = get_smi_port();
	memset(&io_context, 0, sizeof(AMIFLDRV_PORTRW));
	io_context.Port = smi_port;
	io_context.Value.ValueByte = AMI_SMM_BUFFER_INTERACT_SMI;
	io_context.CpuContext.Ecx = AMI_SMM_BUFFER_ACCESS_LOCK;
	io_context.CpuContext.Edi = AMI_SMM_BUFFER_ACCESS_FLAG_NORMAL;
	io_context.CpuContext.Esi = SMM_BUFFER_SIZE;

	int_write8(&io_context);

	// try again with force flag
	if (0 != io_context.CpuContext.Ecx) {
		io_context.Port = smi_port;
		io_context.Value.ValueByte = AMI_SMM_BUFFER_INTERACT_SMI;
		io_context.CpuContext.Ecx = AMI_SMM_BUFFER_ACCESS_LOCK;
		io_context.CpuContext.Edi = AMI_SMM_BUFFER_ACCESS_FLAG_UNLOCK;
		io_context.CpuContext.Esi = SMM_BUFFER_SIZE;
		int_write8(&io_context);

		// fail to lock buffer
		if (0 != io_context.CpuContext.Ecx)
			return 0;
	}

	// continue to locate SMM buffer via mailbox
	smm_mailbox = io_context.CpuContext.Esi;
	mailbox = (SMI_MAILBOX *)ioremap_cache(smm_mailbox, PAGE_SIZE);
	if (mailbox) {
		smm_buffer = mailbox->BufferAddress;
		iounmap((void *)mailbox);
	}
	//print_info("Got mailbox(0x%lx) and buffer(0x%lx)", smm_mailbox, smm_buffer);
	print_info("Got WSMT V1 Success.");

	// unlock smm buffer
	io_context.Port = smi_port;
	io_context.Value.ValueByte = AMI_SMM_BUFFER_INTERACT_SMI;
	io_context.CpuContext.Ecx = AMI_SMM_BUFFER_ACCESS_UNLOCK;
	int_write8(&io_context);
	
	SwSmiPort = smi_port;
	
	return 1;
}

int amismmbufferv1_filter(unsigned long start, unsigned long length)
{
	int page_smm_mailbox = smm_mailbox >> PAGE_SHIFT;
	int page_smm_buffer = smm_buffer >> PAGE_SHIFT;

	// system not support
	if (smm_mailbox == 0 && smm_buffer == 0) return 0;

	// valid mmap() request
	if (start == page_smm_mailbox && length == PAGE_SIZE) return 1;
	if (start == page_smm_buffer && length == SMM_BUFFER_SIZE) return 1;

	print_warn("mmap() request is invalid!");
	// invalid mmap() request
	return -1;
}
