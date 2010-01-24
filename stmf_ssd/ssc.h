/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

#ifndef _SSC_H
#define	_SSC_H

#include <sys/types.h>
#include <sys/stmf.h>
#include <sys/lpif.h>

#include "stmf_ssd.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define	SSD_UNIQUE_ID	0x475353

typedef struct {
	uint32_t	flags;		/* Task flags */
	uint8_t		status;		/* Completion SCSI status */
	uint32_t	saa;		/* Additional status info */
} ssc_command_t;

#define	SSC_TASK_STATUS_PENDING		0x00000001
#define	SSC_TASK_ABORTED		0x00000002

extern stmf_status_t lu_create(stmfssd_state_t *);
extern void lu_remove(stmfssd_state_t *);

typedef struct {
	uint8_t		cdb0;
	void		(*scsi_cmd_handler)(scsi_task_t *, stmf_data_buf_t *);
} scsi_cmd_entry_t;

extern scsi_cmd_entry_t	scsi_cmd_table[];

extern void ssc_send_data(scsi_task_t *, stmf_data_buf_t *, void *, size_t);

/* Forward declaration for SCSI command handlers */
extern void ssc_test_unit_ready(scsi_task_t *, stmf_data_buf_t *);
extern void ssc_request_sense(scsi_task_t *, stmf_data_buf_t *);
extern void ssc_format(scsi_task_t *, stmf_data_buf_t *);
extern void ssc_inquiry(scsi_task_t *, stmf_data_buf_t *);
extern void ssc_erase(scsi_task_t *, stmf_data_buf_t *);
extern void ssc_read_capacity10(scsi_task_t *, stmf_data_buf_t *);
extern void ssc_read(scsi_task_t *, stmf_data_buf_t *);
extern void ssc_write(scsi_task_t *, stmf_data_buf_t *);

#ifdef	__cplusplus
}
#endif

#endif	/* _SSC_H */
