/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * READ (0x08) command implementation
 * See SSC-2 6.4 READ command (pp 62)
 */

#include <sys/types.h>
#include <sys/stmf.h>
#include <sys/lpif.h>
#include <sys/scsi/scsi.h>
#include <sys/scsi/impl/spc3_types.h>

#include <sys/ddi.h>
#include <sys/sunddi.h>

#include "ssc.h"

void
ssc_read(scsi_task_t *task, stmf_data_buf_t *dbuf)
{
	ssc_command_t	*scmd;

	ASSERT(task);
	ASSERT(dbuf);

	scmd = task->task_lu_private;

	VERIFY(scmd);

	scmd->status = SAM4_STATUS_GOOD;
	scmd->saa = 0;
	scmd->flags &= ~SSC_TASK_STATUS_PENDING;

	stmf_scsilib_send_status(task, scmd->status, scmd->saa);
}
