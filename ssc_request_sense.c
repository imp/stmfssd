/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * SCSI REQUEST_SENSE (0x03) command implementation
 */

#include <sys/types.h>
#include <sys/stmf.h>
#include <sys/lpif.h>
#include <sys/scsi/scsi.h>

#include <sys/ddi.h>
#include <sys/sunddi.h>

#include "ssc.h"

void
ssc_request_sense(scsi_task_t *task, stmf_data_buf_t *dbuf)
{
	ssc_command_t	*scmd;

	scmd = task->task_lu_private;

	scmd->status = STATUS_GOOD;
	scmd->saa = 0;
}
