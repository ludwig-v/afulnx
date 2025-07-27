//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amifldrv.c
 *
 */

#include "amifldrv.h"

static int Major;
extern unsigned short SwSmiPort;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
static int device_open_count = 0;
#endif

static int amifldrv_open(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
	MOD_INC_USE_COUNT;
#else
	if (device_open_count) return -EBUSY;

	device_open_count++;
	try_module_get(THIS_MODULE);
#endif
	return 0;
}

static int amifldrv_release(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
	MOD_DEC_USE_COUNT;
#else
	device_open_count--;
	module_put(THIS_MODULE);
#endif
	return 0;
}

static long amifldrv_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	AMIFLDRV_PORTRW		arg_kernel;
	unsigned short		port;	
	unsigned long 		result;
	
	result = copy_from_user((void *)&arg_kernel, (void *)arg, sizeof(AMIFLDRV_PORTRW));
	port = arg_kernel.Port;	

	switch(cmd) {
		case CMD_ALLOC :
			//return amimemdrv_alloc((void *)arg);
			break;
		case CMD_FREE :
			//return amimemdrv_free((void *)arg);
			break;
		case CMD_IOWRITE_BYTE :		
			if (!io_port_filter(port)) break;
			return amiio_write8((void *)arg);

		case CMD_IOWRITE_WORD :
			if (!io_port_filter(port)) break;
			return amiio_write16((void *)arg);

		case CMD_IOWRITE_DWORD :
			if (!io_port_filter(port)) break;
			return amiio_write32((void *)arg);

		case CMD_IOREAD_BYTE :
			if (!io_port_filter(port)) break;
			return amiio_read8((void *)arg);

		case CMD_IOREAD_WORD :
			if (!io_port_filter(port)) break;
			return amiio_read16((void *)arg);

		case CMD_IOREAD_DWORD :
			if (!io_port_filter(port)) break;
			return amiio_read32((void *)arg);

		case CMD_LOCK_KB:
		//	disable_irq(1);
			return 0;

		case CMD_UNLOCK_KB:
		//	enable_irq(1);
			return 0;

		case CMD_GET_DRIVER_INFO :
			return amidrv_getinfo((void *)arg);
	}

	return -ENOTTY;
}

static int amifldrv_mmap(struct file *file, struct vm_area_struct *vma)
{
	int				status = -EINVAL;
	unsigned long	size = vma->vm_end - vma->vm_start;

	if (-1 == amismmbufferv1_filter(vma->vm_pgoff, size)) return -EINVAL;
	if (-1 == amismmbufferv2_filter(vma->vm_pgoff, size)) return -EINVAL;

	if ((vma->vm_flags & VM_WRITE) && !(vma->vm_flags & VM_SHARED)) return -EINVAL;
	vma->vm_flags |= VM_LOCKED;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0))
	status = remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, size, PAGE_SHARED);
#else
	status = remap_page_range(vma, vma->vm_start, vma->vm_pgoff << PAGE_SHIFT, size, PAGE_SHARED);
#endif

	return status ? -ENXIO : 0;
}

static struct file_operations amifldrv_fops =
{
	owner			: THIS_MODULE,
	open			: amifldrv_open,
	release			: amifldrv_release,
	unlocked_ioctl	: amifldrv_unlocked_ioctl,
	mmap			: amifldrv_mmap
};

static int amifldrv_init_module(void)
{
	Major = register_chrdev(0, AMIFLDRV_NAME, &amifldrv_fops);
	if (Major < 0) return (-EIO);

	// initialize physical memory module
	SwSmiPort = 0;
	amimemdrv_init();
	amismmbufferv1_init();
	amismmbufferv2_init();

	return 0;
}

static void amifldrv_cleanup_module(void)
{
	// release allocated resources
	amimemdrv_release();

	unregister_chrdev(Major, AMIFLDRV_NAME);
}

module_init(amifldrv_init_module);
module_exit(amifldrv_cleanup_module);
