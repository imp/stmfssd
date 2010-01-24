/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * STMF interface implementation
 */

#include <sys/types.h>
#include <sys/stmf.h>
#include <sys/lpif.h>
#include <sys/stmf_ioctl.h>
#include <sys/scsi/scsi.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>

#include "ssc.h"

#define	SSC_ABORT_TIMEOUT	16

static stmf_status_t
ssc_task_alloc(scsi_task_t *task)
{
	task->task_lu_private = kmem_zalloc(sizeof (ssc_command_t), KM_NOSLEEP);

	return ((task->task_lu_private != NULL) ?
	    (STMF_SUCCESS) : (STMF_ALLOC_FAILURE));
}

static void
ssc_task_free(scsi_task_t *task)
{
	ASSERT(task->task_lu_private != NULL);

	if (task->task_lu_private != NULL) {
		/* XXX May need to check flags in dad_command_t before free */
		kmem_free(task->task_lu_private, sizeof (ssc_command_t));
		task->task_lu_private = NULL;
	}
}

/*
 * That is a central place to handle everything SCSI-related
 */
static void
ssc_new_task(scsi_task_t *task, stmf_data_buf_t *dbuf)
{
	ssc_command_t	*scmd;

	scmd = task->task_lu_private;
	scmd->flags |= SSC_TASK_STATUS_PENDING;

	/* XXX Register this task within the ITL */

	/* First take care of management function */
	if (task->task_mgmt_function) {
		stmf_scsilib_handle_task_mgmt(task);
		return;
	}

	/* Now route the SCSI command to appropriate handler */
	scsi_cmd_table[task->task_cdb[0]].scsi_cmd_handler(task, dbuf);
}

static void
ssc_dbuf_xfer_done(scsi_task_t *task, stmf_data_buf_t *dbuf)
{
	ssc_command_t	*scmd;

	ASSERT(task);
	ASSERT(dbuf);

	scmd = task->task_lu_private;
	VERIFY(scmd);

	if (dbuf->db_xfer_status != STMF_SUCCESS) {
		stmf_abort(STMF_QUEUE_TASK_ABORT, task,
		    dbuf->db_xfer_status, NULL);
		return;
	}

	if (scmd->flags & SSC_TASK_STATUS_PENDING) {
		task->task_nbytes_transferred = dbuf->db_data_size;
		stmf_scsilib_send_status(task, scmd->status, scmd->saa);
	}
}

static void
ssc_send_status_done(scsi_task_t *task)
{

}

static stmf_status_t
ssc_abort(stmf_lu_t *lup, int abort_cmd, void *arg, uint32_t flags)
{
	return (STMF_SUCCESS);
}

static void
ssc_ctl(stmf_lu_t *lup, int cmd, void *arg)
{

}

static stmf_status_t
ssc_info(uint32_t cmd, stmf_lu_t *lup, void *arg, uint8_t *buf,
    uint32_t *bufsizep)
{
	return (STMF_NOT_SUPPORTED);
}

stmf_status_t
lu_create(stmfssd_state_t *sp)
{
	stmf_lu_t		*lup;
	scsi_devid_desc_t	*devid = (scsi_devid_desc_t *)sp->did;

	lup = stmf_alloc(STMF_STRUCT_STMF_LU, 0, 0);
	if (lup == NULL) {
		return (STMF_FAILURE);
	}

	devid->ident_length = 0x10;
	if (stmf_scsilib_uniq_lu_id(SSD_UNIQUE_ID,
	    devid) != STMF_SUCCESS) {
		stmf_free(lup);
		return (STMF_FAILURE);
	}
	lup->lu_provider_private = sp;	/* Keep a back pointer for later use */
	lup->lu_id = devid;
	lup->lu_alias = (char *)refstr_value(sp->dev);
	lup->lu_lp = sp->lpp;
	lup->lu_abort_timeout = SSC_ABORT_TIMEOUT;
	lup->lu_task_alloc = ssc_task_alloc;
	lup->lu_new_task = ssc_new_task;
	lup->lu_dbuf_xfer_done = ssc_dbuf_xfer_done;
	lup->lu_send_status_done = ssc_send_status_done;
	lup->lu_task_free = ssc_task_free;
	lup->lu_abort = ssc_abort;
	lup->lu_ctl = ssc_ctl;
	lup->lu_info = ssc_info;

	if (stmf_register_lu(lup) == STMF_SUCCESS) {
		sp->lup = lup;
		return (STMF_SUCCESS);
	} else {
		stmf_free(lup);
		return (STMF_FAILURE);
	}
}

void
lu_remove(stmfssd_state_t *sp)
{
	/* XXX Make sure there is no pending IO on this LU */
	if (stmf_deregister_lu(sp->lup) != STMF_SUCCESS) {
		/* Check what's wrong and retry */
	}
	stmf_free(sp->lup);
	sp->lup = NULL;
}
