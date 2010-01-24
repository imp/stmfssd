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

#define	STMFSSD_IOCTL		0x53534400	/* "SSDx" */
#define	STMFSSD_GET_VERSION	(STMFSSD_IOCTL | 0)
#define	STMFSSD_LIST_DEV	(STMFSSD_IOCTL | 1)
#define	STMFSSD_CREATE_DEV	(STMFSSD_IOCTL | 2)
#define	STMFSSD_REMOVE_DEV	(STMFSSD_IOCTL | 3)

#define	STMFSSD_ABI_VERSION_0	0
#define	STMFSSD_ABI_VERSION_1	1
#define	STMFSSD_ABI_VERSION	STMFSSD_ABI_VERSION_1

typedef struct {
	uint32_t	version;	/* ABI version    */
	uint32_t	subcode;	/* Operation subcode, if any */
	size_t		len;		/* Payload length */
} stmfssd_cmd_t;

typedef struct {
	uint32_t	version;
	uint32_t	status;
	size_t		len;
} stmfssd_reply_t;

typedef struct {
	char		vendor[8];	/* INQUIRY Vendor ID */
	char		product[16];	/* INQUIRY Product ID */
	char		fwrev[4];	/* INQUIRY FW Revision */
} stmfssd_create_dev_cmd_t;

typedef struct {
	minor_t		minor;		/* Minor node to remove */
} stmfssd_remove_dev_cmd_t;

#ifdef	__cplusplus
}
#endif

#endif	/* _STMF_SSD_IOCTL_H */
