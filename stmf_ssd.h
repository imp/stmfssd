/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _STMF_SSD_H
#define	_STMF_SSD_H

#include <sys/stmf.h>
#include <sys/lpif.h>
#include <sys/refstr.h>
#include <sys/devctl.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
	dev_info_t		*dip;	/* devinfo */
	stmf_lu_provider_t	*lpp;	/* LU provider */
	stmf_lu_t		*lup;	/* LU vector */
	refstr_t		*dev;	/* Target device name */

	/* SCSI-related properties */
        char                    vid[8];
        char                    pid[16];
        char                    fwrev[4];
        size_t			size;	/* Device size in logical blocks */
	size_t			blocksize;	/* Block size (512 bytes) */
	uint8_t			did[16];	/* SPC-3 SCSI device id */
} stmf_ssd_state_t;

#ifdef	__cplusplus
}
#endif

#endif	/* _STMF_SSD_H */
