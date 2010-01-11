/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _STMF_SSD_IOCTL_H
#define	_STMF_SSD_IOCTL_H

#include <sys/types.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define	STMF_SSD_IOCTL			0x53534400	/* "SSDx" */
#define	STMF_SSD_GET_VERSION		(STMF_SSD_IOCTL | 0)
#define	STMF_SSD_LIST_DEV		(STMF_SSD_IOCTL | 1)
#define	STMF_SSD_CREATE_DEV		(STMF_SSD_IOCTL | 2)
#define	STMF_SSD_REMOVE_DEV		(STMF_SSD_IOCTL | 3)

#define	STMF_SSD_ABI_VERSION_0	0
#define	STMF_SSD_ABI_VERSION_1	1
#define	STMF_SSD_ABI_VERSION		STMF_SSD_ABI_VERSION_1

typedef struct {
	uint32_t	version;	/* ABI version    */
	uint32_t	opcode;		/* Operation code */
	size_t		len;		/* Payload length */
} stmf_ssd_cmd_t;

typedef struct {
	uint32_t	version;
	uint32_t	status;
	size_t		len;
} stmf_ssd_reply_t;

typedef struct {
	int		devtype;	/* Emulated tape type */
	char		vendor[8];	/* INQUIRY Vendor ID */
	char		product[16];	/* INQUIRY Product ID */
	char		fwrev[4];	/* INQUIRY FW Revision */
	int		unused1;	/* Placeholder */
	int		unused2;	/* Placeholder */
	char		dev[1];		/* The unique device name */
} stmf_ssd_create_dev_cmd_t;

typedef struct {
	minor_t		minor;		/* Minor node to remove */
        char            dev[1];         /* Device name to remove */
} stmf_ssd_remove_dev_cmd_t;

#ifdef	__cplusplus
}
#endif

#endif	/* _STMF_SSD_IOCTL_H */
