//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amismmbufferv2.c
 *
 */

#include <linux/acpi.h>
#include <linux/efi.h>
#include <acpi/actbl2.h>

#include "amifldrv.h"

#pragma pack(1)
struct acpi_table_uefi_smm_comm {
	struct acpi_table_uefi	acpi_table_uefi_hdr;
	u16						rev;
	u64						mailbox_addr;
	u32						smi_code;
	u32						mailbox_hdr_size;
};
#pragma pack()

extern int int_write8(AMIFLDRV_PORTRW *);
extern unsigned short SwSmiPort;

static const char acpi_uefi_path[] = "/sys/firmware/acpi/tables/UEFI";
static const efi_guid_t GuidNonRegisterBasedSwSmiInterface = EFI_GUID(0xbaedb05d, 0xf2ce, 0x485b, 0xb4, 0x54, 0xc2, 0x51, 0x87, 0xc, 0xde, 0xfc);

static unsigned long smm_mailbox = 0;
static unsigned long smm_buffer = 0;

static int int_write8_wrapper(unsigned short port, unsigned char value)
{
	AMIFLDRV_PORTRW io_context;

	memset(&io_context, 0, sizeof(AMIFLDRV_PORTRW));
	io_context.Port = port;
	io_context.Value.ValueByte = value;

	return int_write8(&io_context);
}

int amismmbufferv2_init(void)
{
	struct file *uefi;
	struct acpi_table_uefi_smm_comm uefi_table;

	unsigned char smi_number = 0;
	unsigned short smi_port = get_smi_port();
	SMM_COMM_MAILBOX *mailbox = 0;
	loff_t pos = 0;

	// retrieve acpi uefi table for mailbox address
	memset((void *)&uefi_table, 0, sizeof(uefi_table));
	uefi = filp_open(acpi_uefi_path, O_RDONLY, 0);
	if (IS_ERR(uefi)) return 0;
	kernel_read(uefi, (void *)&uefi_table, sizeof(uefi_table), &pos);
	filp_close(uefi, 0);

	// now validate the GUID
	if (0 != memcmp(&(uefi_table.acpi_table_uefi_hdr.identifier), (void *)&GuidNonRegisterBasedSwSmiInterface, 16))
		return 0;

	smi_number = (unsigned char)(uefi_table.smi_code);
	smm_mailbox = uefi_table.mailbox_addr;
	mailbox = (SMM_COMM_MAILBOX *)ioremap_cache(smm_mailbox, PAGE_SIZE);
	if (mailbox) {
		mailbox->MailBox.Status		= AMI_SMM_BUFFER_ACCESS_LOCK;
		mailbox->Params.Lock.Size	= SMM_BUFFER_SIZE;
		mailbox->Params.Lock.Flags	= AMI_SMM_BUFFER_ACCESS_FLAG_NORMAL;
		int_write8_wrapper(smi_port, smi_number);

		// try again with force flag
		if (0 != mailbox->MailBox.Status) {
			mailbox->MailBox.Status		= AMI_SMM_BUFFER_ACCESS_LOCK;
			mailbox->Params.Lock.Size	= SMM_BUFFER_SIZE;
			mailbox->Params.Lock.Flags	= AMI_SMM_BUFFER_ACCESS_FLAG_UNLOCK;
			int_write8_wrapper(smi_port, smi_number);
		}

		// return failure, no smm bffer available from now after
		if (0 != mailbox->MailBox.Status) {
			iounmap((void *)mailbox);
			return 0;
		}

		smm_buffer = mailbox->MailBox.BufferAddress;

		// unlock smm buffer
		mailbox->MailBox.Status = AMI_SMM_BUFFER_ACCESS_UNLOCK;
		int_write8_wrapper(smi_port, smi_number);

		iounmap((void *)mailbox);
	}
	//print_info("Got mailbox(0x%lx) and buffer(0x%lx)", smm_mailbox, smm_buffer);
	print_info("Got WSMT V2 Success.");
	
	SwSmiPort = smi_port;

	return 1;
}

int amismmbufferv2_filter(unsigned long start, unsigned long length)
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
