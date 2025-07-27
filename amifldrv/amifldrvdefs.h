//***********************************************************************
//*                                                                     *
//*   Copyright (c) 1985-2022, American Megatrends International LLC.   *
//*                                                                     *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                                                                     *
//***********************************************************************

/*
 * @file amifldrvdefs.h
 *
 */

#ifndef _AMIFLDRVDEFS__H_
#define _AMIFLDRVDEFS__H_

#define CMD_ALLOC				0x4160
#define CMD_FREE				0x4161
#define CMD_LOCK_KB				0x4162
#define CMD_UNLOCK_KB			0x4163
#define CMD_IOWRITE_BYTE		0x4164
#define CMD_IOWRITE_WORD		0x4165
#define CMD_IOWRITE_DWORD		0x4166
#define CMD_IOREAD_BYTE			0x4167
#define CMD_IOREAD_WORD			0x4168
#define CMD_IOREAD_DWORD		0x4169
#define CMD_GET_DRIVER_INFO		0x416A

#define SMM_BUFFER_SIZE			0x11000		// 64KB + 4KB

#define AMI_SMM_BUFFER_INTERACT_SMI		0xD9	// before rev.2

#define AMI_SMM_BUFFER_ACCESS_LOCK		0xC0000001
#define AMI_SMM_BUFFER_ACCESS_UNLOCK	0xC0000002
#define AMI_SMM_BUFFER_SMM_COMM_REQ		0xC0000003
#define AMI_SMM_BUFFER_COMM_X86_REGS	0xC0000004

#define	AMI_SMM_BUFFER_ACCESS_FLAG_NORMAL	0
#define	AMI_SMM_BUFFER_ACCESS_FLAG_UNLOCK	1

/*
 * 2. Data structures
 *
 */
#pragma pack(1)
typedef struct tagAMIFLDRV_ALLOC
{
	long			size;
	unsigned long	kvirtlen;
	void			*kmallocptr;
	void			*kvirtadd;
	void			*kphysadd;
} AMIFLDRV_ALLOC;

typedef struct tagAMIFLDRV_CPU_CONTEXT
{
	unsigned int		Edi;
	unsigned int		Esi;
	unsigned int		Ebp;
	unsigned int		Ebx;
	unsigned int		Edx;
	unsigned int		Ecx;
	unsigned int		Eax;
} AMIFLDRV_CPU_CONTEXT;

typedef struct tagAMIFLDRV_PORTRW
{
	union {
		unsigned char		ValueByte;
		unsigned short		ValueWord;
		unsigned int		ValueDword;
	} Value;
	unsigned short			Port;
	AMIFLDRV_CPU_CONTEXT	CpuContext;
} AMIFLDRV_PORTRW;

typedef struct tagSMI_MAILBOX {
	unsigned long		Revision;		///< Revision of this data structure. Will be changed to 2
	unsigned long		BufferAddress;	///< Physical address of the SMM Communication buffer. No change
	unsigned int		Status;			///< Status of the last SMI call. When revision is 2, treated as command ID on input and as status on output
	unsigned int		Token;			///< Token that identifies SMI caller. No change
} SMI_MAILBOX;

typedef struct tagSMM_COMM_BUFFER_LOCK_PARAMS {
	unsigned int		Size;
	unsigned int		Flags;
} SMM_COMM_BUFFER_LOCK_PARAMS;

typedef struct tagSMM_COMM_BUFFER_X86_REGISTERS {
	unsigned int		SwSmiNumber;
	unsigned int		Reserved;
	unsigned long		Rax;
	unsigned long		Rbx;
	unsigned long		Rcx;
	unsigned long		Rdx;
	unsigned long		Rsi;
	unsigned long		Rdi;
} SMM_COMM_BUFFER_X86_REGISTERS;

typedef struct tagSMM_COMM_MAILBOX {
	SMI_MAILBOX		MailBox;

	union {
		SMM_COMM_BUFFER_LOCK_PARAMS		Lock;
		SMM_COMM_BUFFER_X86_REGISTERS	Regs;
	} Params;
} SMM_COMM_MAILBOX;

typedef struct tagAMIDRV_INFO {
	unsigned int	InfoVersion;
	unsigned int	InfoLength;
	unsigned int	Major;
	unsigned int	Minor;
} AMIDRV_INFO;
#pragma pack()

#endif	// _AMIFLDRVDEFS__H_
