/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * SCSI INQUIRY (0x12) command implementation
 */

#include <sys/types.h>
#include <sys/byteorder.h>
#include <sys/debug.h>
#include <sys/stmf.h>
#include <sys/lpif.h>
#include <sys/scsi/scsi.h>
#include <sys/scsi/impl/spc3_types.h>
#include <sys/scsi/generic/inquiry.h>
#include <sys/ddi.h>
#include <sys/sunddi.h>

#include "ssc.h"

/*
 * TODO: Need to evaluate the option of poking the data directly into
 *       stmf_data_buf_t, skipping the 'buf' allocation.
 */

void
ssc_inquiry(scsi_task_t *task, stmf_data_buf_t *dbuf)
{
	stmf_ssd_state_t	*sp;
	ssc_command_t		*scmd;
	spc3_inquiry_cdb_t	*inq;
	void			*buf;
	size_t			len;

	scmd = task->task_lu_private;
	sp = task->task_lu->lu_provider_private;

	VERIFY(task->task_cdb_length == sizeof (spc3_inquiry_cdb_t));

	/* Prepare the status for successful completion */
	scmd->status = SAM4_STATUS_GOOD;
	scmd->saa = 0;

	inq = (spc3_inquiry_cdb_t *)task->task_cdb;

	/* Sanity checks according to SPC-3 6.4.1 INQUIRY */
	if ((inq->ic_evpd == 0) && (inq->ic_page_code != 0)) {
		scmd->status = SAM4_STATUS_CHECK_CONDITION;
		scmd->saa = STMF_SAA_INVALID_FIELD_IN_CDB;
		return;
	}

	if ((inq->ic_evpd == 0) && (BE_16(inq->ic_allocation_length) < 5) ||
	    (inq->ic_evpd == 1) && (BE_16(inq->ic_allocation_length) < 4)) {
		scmd->status = SAM4_STATUS_CHECK_CONDITION;
		scmd->saa = STMF_SAA_INVALID_FIELD_IN_CDB;
		return;
	}

	/* OK, the command seems to be all right, let's get some answer */

	if (inq->ic_evpd == 0) {
		/* Standard INQUIRY data */
		spc3_inquiry_data_t	*sid;	/* Standard INQUIRY data */

		len = 36;			/* XXX For now - fix it later */
		buf = kmem_zalloc(len, KM_SLEEP);
		sid = buf;	/* XXX beware, the structure is greater then */
				/* 36 bytes */

		sid->id_peripheral_device_type = SPC3_DEVTYPE_SEQUENTIAL;
		sid->id_version = 5;				/* SPC-3 */
		sid->id_response_data_format = RDF_SCSI2;	/* SPC-3 */
		sid->additional_length = BE_16(len - 5);
		sid->id_tpgs = 1;
		sid->id_cmd_que = 1;
		bcopy(sp->vid, sid->id_vendor_id, 8);
		bcopy(sp->pid, sid->id_product_id, 16);
		bcopy(sp->fwrev, sid->id_product_revision, 4);
	} else {
		/* EVPD */
		len = 96;
		buf = kmem_zalloc(len, KM_SLEEP);

		/* XXX Leave it zero for now, need to fix EVPD data here */
	}

	/* The reply buffer is ready, send it out */

	size_t xfer_size = min(len, inq->ic_allocation_length);

	ssc_send_data(task, dbuf, buf, xfer_size);

#ifdef XXX
	task->task_cmd_xfer_length = xfer_size;
	if (task->task_additional_flags & TASK_AF_NO_EXPECTED_XFER_LENGTH) {
		/* XXX Not sure why is it needed - I see this in sbd driver */
	}

	if (dbuf == NULL) {
		uint32_t	minsize = xfer_size;
		dbuf = stmf_alloc_dbuf(task, xfer_size, &minsize, 0);
	}

	if (dbuf != NULL) {
		/* We have the requested buffer, now copy the data into it */

		VERIFY(dbuf->db_buf_size >= xfer_size);
		/* XXX For now only cope with single element of sglist */
		VERIFY(dbuf->db_sglist[0].seg_length >= xfer_size);

		bcopy(buf, dbuf->db_sglist[0].seg_addr, xfer_size);
		dbuf->db_data_size = xfer_size;
		dbuf->db_relative_offset = 0;
		dbuf->db_flags = DB_DIRECTION_TO_RPORT;

		stmf_xfer_data(task, dbuf, 0);
	} else {
		/* If no luck allocating data buffer just abort */
		stmf_abort(STMF_QUEUE_TASK_ABORT, task,
		    STMF_ALLOC_FAILURE, NULL);
	}
#endif

	kmem_free(buf, len);
}
