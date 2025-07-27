//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amiiodrv.c
 *
 */

#include "amifldrv.h"

#define IORW_PROLOGUE(V) \
			unsigned long result; \
			result = copy_from_user((void *)&arg_kernel, arg, sizeof(AMIFLDRV_PORTRW)); \
			port = arg_kernel.Port; \
			value = arg_kernel.Value.V; \
			eax = arg_kernel.CpuContext.Eax; \
			ebx = arg_kernel.CpuContext.Ebx; \
			ecx = arg_kernel.CpuContext.Ecx; \
			edx = arg_kernel.CpuContext.Edx; \
			esi = arg_kernel.CpuContext.Esi; \
			edi = arg_kernel.CpuContext.Edi;

#define IORW_EPILOGUE() \
			arg_kernel.CpuContext.Eax = eax; \
			arg_kernel.CpuContext.Ebx = ebx; \
			arg_kernel.CpuContext.Ecx = ecx; \
			arg_kernel.CpuContext.Edx = edx; \
			arg_kernel.CpuContext.Esi = esi; \
			arg_kernel.CpuContext.Edi = edi; \
			result = copy_to_user(arg, (void *)&arg_kernel, sizeof(AMIFLDRV_PORTRW));

int amiio_write8(void *arg)
{
	AMIFLDRV_PORTRW		arg_kernel;
	unsigned short		port;
	unsigned char		value;
	unsigned int		eax, ebx, ecx, edx, esi, edi;

	IORW_PROLOGUE(ValueByte);
	if (!io_port_filter(port)) return -EINVAL;
	__asm__ (
		"movb	%12, %%al\n"
		"outb	%%al, %%dx\n"
		:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		:"a"(eax), "b"(ebx), "c"(ecx), "d"(port), "S"(esi), "D"(edi), "m"(value)
	);
	IORW_EPILOGUE();

	return 0;
}

int amiio_write16(void *arg)
{
	AMIFLDRV_PORTRW		arg_kernel;
	unsigned short		port;
	unsigned short		value;
	unsigned int		eax, ebx, ecx, edx, esi, edi;

	IORW_PROLOGUE(ValueWord);
	if (!io_port_filter(port)) return -EINVAL;
	__asm__ (
		"movw	%12, %%ax\n"
		"outw	%%ax, %%dx\n"
		:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		:"a"(eax), "b"(ebx), "c"(ecx), "d"(port), "S"(esi), "D"(edi), "m"(value)
	);
	IORW_EPILOGUE();

	return 0;
}

int amiio_write32(void *arg)
{
	AMIFLDRV_PORTRW		arg_kernel;
	unsigned short		port;
	unsigned long		value;
	unsigned int		eax, ebx, ecx, edx, esi, edi;

	IORW_PROLOGUE(ValueDword);
	if (!io_port_filter(port)) return -EINVAL;
	__asm__ (
		"movl	%12, %%eax\n"
		"outl	%%eax, %%dx\n"
		:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		:"a"(eax), "b"(ebx), "c"(ecx), "d"(port), "S"(esi), "D"(edi), "m"(value)
	);
	IORW_EPILOGUE();

	return 0;
}

int amiio_read8(void *arg)
{
	AMIFLDRV_PORTRW		arg_kernel;
	unsigned short		port;
	unsigned char		value;
	unsigned int		eax, ebx, ecx, edx, esi, edi;

	IORW_PROLOGUE(ValueByte);
	if (!io_port_filter(port)) return -EINVAL;
	__asm__ (
		"inb	%%dx, %%al\n"
		:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		:"a"(eax), "b"(ebx), "c"(ecx), "d"(port), "S"(esi), "D"(edi)
	);
	IORW_EPILOGUE();

	return 0;
}

int amiio_read16(void *arg)
{
	AMIFLDRV_PORTRW		arg_kernel;
	unsigned short		port;
	unsigned short		value;
	unsigned int		eax, ebx, ecx, edx, esi, edi;

	IORW_PROLOGUE(ValueWord);
	if (!io_port_filter(port)) return -EINVAL;
	__asm__ (
		"inw	%%dx, %%ax\n"
		:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		:"a"(eax), "b"(ebx), "c"(ecx), "d"(port), "S"(esi), "D"(edi)
	);
	IORW_EPILOGUE();

	return 0;
}

int amiio_read32(void *arg)
{
	AMIFLDRV_PORTRW		arg_kernel;
	unsigned short		port;
	unsigned int		value;
	unsigned int		eax, ebx, ecx, edx, esi, edi;

	IORW_PROLOGUE(ValueDword);
	if (!io_port_filter(port)) return -EINVAL;
	__asm__ (
		"inl	%%dx, %%eax\n"
		:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		:"a"(eax), "b"(ebx), "c"(ecx), "d"(port), "S"(esi), "D"(edi)
	);
	IORW_EPILOGUE();

	return 0;
}

// only for smmbuffer protocol
int int_write8(AMIFLDRV_PORTRW *arg)
{
	unsigned short		port;
	unsigned char		value;
	unsigned int		eax, ebx, ecx, edx, esi, edi;

	port = arg->Port;

	if (!io_port_filter(port)) return -EINVAL;

	value = arg->Value.ValueByte;

	eax = arg->CpuContext.Eax;
	ebx = arg->CpuContext.Ebx;
	ecx = arg->CpuContext.Ecx;
	edx = arg->CpuContext.Edx;
	esi = arg->CpuContext.Esi;
	edi = arg->CpuContext.Edi;

	__asm__ (
		"movb	%12, %%al\n"
		"outb	%%al, %%dx\n"
		:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi)
		:"a"(eax), "b"(ebx), "c"(ecx), "d"(port), "S"(esi), "D"(edi), "m"(value)
	);

	arg->CpuContext.Eax = eax;
	arg->CpuContext.Ebx = ebx;
	arg->CpuContext.Ecx = ecx;
	arg->CpuContext.Edx = edx;
	arg->CpuContext.Esi = esi;
	arg->CpuContext.Edi = edi;

	return 0;
}
