/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * SSC command router
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
ssc_invalid_cmd(scsi_task_t *task, stmf_data_buf_t *dbuf)
{
	ssc_command_t	*scmd;

	scmd = task->task_lu_private;

	scmd->status = SAM4_STATUS_CHECK_CONDITION;
	scmd->saa = STMF_SAA_INVALID_OPCODE;
}

void
ssc_send_data(scsi_task_t *task, stmf_data_buf_t *dbuf,
    void *data, size_t xfer_size)
{
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

		bcopy(data, dbuf->db_sglist[0].seg_addr, xfer_size);
		dbuf->db_data_size = xfer_size;
		dbuf->db_relative_offset = 0;
		dbuf->db_flags = DB_DIRECTION_TO_RPORT;

		stmf_xfer_data(task, dbuf, 0);
	} else {
		/* If no luck allocating data buffer just abort */
		stmf_abort(STMF_QUEUE_TASK_ABORT, task,
		    STMF_ALLOC_FAILURE, NULL);
	}
}

scsi_cmd_entry_t	scsi_cmd_table[] = {
	{SPC3_CMD_TEST_UNIT_READY,	ssc_test_unit_ready},
	{0x01,				ssc_invalid_cmd},
	{0x02,				ssc_invalid_cmd},
	{SPC3_CMD_REQUEST_SENSE,	ssc_request_sense},
	{SPC3_CMD_FORMAT,		ssc_invalid_cmd},
	{0x05,				ssc_invalid_cmd},
	{0x06,				ssc_invalid_cmd},
	{0x07,				ssc_invalid_cmd},
	{SPC3_CMD_READ6,		ssc_read},
	{0x09,				ssc_invalid_cmd},
	{SPC3_CMD_WRITE6,		ssc_write},
	{0x0B,				ssc_invalid_cmd},
	{0x0C,				ssc_invalid_cmd},
	{0x0D,				ssc_invalid_cmd},
	{0x0E,				ssc_invalid_cmd},
	{0x0F,				ssc_invalid_cmd},

	{0x10,				ssc_invalid_cmd},
	{0x11,				ssc_invalid_cmd},
	{SPC3_CMD_INQUIRY,		ssc_inquiry},
	{0x13,				ssc_invalid_cmd},
	{0x14,				ssc_invalid_cmd},
	{0x15,				ssc_invalid_cmd},
	{0x16,				ssc_invalid_cmd},
	{0x17,				ssc_invalid_cmd},
	{0x18,				ssc_invalid_cmd},
	{SPC3_CMD_ERASE6,		ssc_erase},
	{0x1A,				ssc_invalid_cmd},
	{0x1B,				ssc_invalid_cmd},
	{0x1C,				ssc_invalid_cmd},
	{0x1D,				ssc_invalid_cmd},
	{0x1E,				ssc_invalid_cmd},
	{0x1F,				ssc_invalid_cmd},

	{0x20,				ssc_invalid_cmd},
	{0x21,				ssc_invalid_cmd},
	{0x22,				ssc_invalid_cmd},
	{0x23,				ssc_invalid_cmd},
	{0x24,				ssc_invalid_cmd},
	{0x25,				ssc_invalid_cmd},
	{0x26,				ssc_invalid_cmd},
	{0x27,				ssc_invalid_cmd},
	{SPC3_CMD_READ10,		ssc_read},
	{0x29,				ssc_invalid_cmd},
	{SPC3_CMD_WRITE10,		ssc_write},
	{0x2B,				ssc_invalid_cmd},
	{0x2C,				ssc_invalid_cmd},
	{0x2D,				ssc_invalid_cmd},
	{0x2E,				ssc_invalid_cmd},
	{0x2F,				ssc_invalid_cmd},

	{0x30,				ssc_invalid_cmd},
	{0x31,				ssc_invalid_cmd},
	{0x32,				ssc_invalid_cmd},
	{0x33,				ssc_invalid_cmd},
	{0x34,				ssc_invalid_cmd},
	{0x35,				ssc_invalid_cmd},
	{0x36,				ssc_invalid_cmd},
	{0x37,				ssc_invalid_cmd},
	{0x38,				ssc_invalid_cmd},
	{0x39,				ssc_invalid_cmd},
	{0x3A,				ssc_invalid_cmd},
	{0x3B,				ssc_invalid_cmd},
	{0x3C,				ssc_invalid_cmd},
	{0x3D,				ssc_invalid_cmd},
	{0x3E,				ssc_invalid_cmd},
	{0x3F,				ssc_invalid_cmd},

	{0x40,				ssc_invalid_cmd},
	{0x41,				ssc_invalid_cmd},
	{0x42,				ssc_invalid_cmd},
	{0x43,				ssc_invalid_cmd},
	{0x44,				ssc_invalid_cmd},
	{0x45,				ssc_invalid_cmd},
	{0x46,				ssc_invalid_cmd},
	{0x47,				ssc_invalid_cmd},
	{0x48,				ssc_invalid_cmd},
	{0x49,				ssc_invalid_cmd},
	{0x4A,				ssc_invalid_cmd},
	{0x4B,				ssc_invalid_cmd},
	{0x4C,				ssc_invalid_cmd},
	{0x4D,				ssc_invalid_cmd},
	{0x4E,				ssc_invalid_cmd},
	{0x4F,				ssc_invalid_cmd},

	{0x50,				ssc_invalid_cmd},
	{0x51,				ssc_invalid_cmd},
	{0x52,				ssc_invalid_cmd},
	{0x53,				ssc_invalid_cmd},
	{0x54,				ssc_invalid_cmd},
	{0x55,				ssc_invalid_cmd},
	{0x56,				ssc_invalid_cmd},
	{0x57,				ssc_invalid_cmd},
	{0x58,				ssc_invalid_cmd},
	{0x59,				ssc_invalid_cmd},
	{0x5A,				ssc_invalid_cmd},
	{0x5B,				ssc_invalid_cmd},
	{0x5C,				ssc_invalid_cmd},
	{0x5D,				ssc_invalid_cmd},
	{0x5E,				ssc_invalid_cmd},
	{0x5F,				ssc_invalid_cmd},

	{0x60,				ssc_invalid_cmd},
	{0x61,				ssc_invalid_cmd},
	{0x62,				ssc_invalid_cmd},
	{0x63,				ssc_invalid_cmd},
	{0x64,				ssc_invalid_cmd},
	{0x65,				ssc_invalid_cmd},
	{0x66,				ssc_invalid_cmd},
	{0x67,				ssc_invalid_cmd},
	{0x68,				ssc_invalid_cmd},
	{0x69,				ssc_invalid_cmd},
	{0x6A,				ssc_invalid_cmd},
	{0x6B,				ssc_invalid_cmd},
	{0x6C,				ssc_invalid_cmd},
	{0x6D,				ssc_invalid_cmd},
	{0x6E,				ssc_invalid_cmd},
	{0x6F,				ssc_invalid_cmd},

	{0x70,				ssc_invalid_cmd},
	{0x71,				ssc_invalid_cmd},
	{0x72,				ssc_invalid_cmd},
	{0x73,				ssc_invalid_cmd},
	{0x74,				ssc_invalid_cmd},
	{0x75,				ssc_invalid_cmd},
	{0x76,				ssc_invalid_cmd},
	{0x77,				ssc_invalid_cmd},
	{0x78,				ssc_invalid_cmd},
	{0x79,				ssc_invalid_cmd},
	{0x7A,				ssc_invalid_cmd},
	{0x7B,				ssc_invalid_cmd},
	{0x7C,				ssc_invalid_cmd},
	{0x7D,				ssc_invalid_cmd},
	{0x7E,				ssc_invalid_cmd},
	{0x7F,				ssc_invalid_cmd},

	{0x80,				ssc_invalid_cmd},
	{0x81,				ssc_invalid_cmd},
	{0x82,				ssc_invalid_cmd},
	{0x83,				ssc_invalid_cmd},
	{0x84,				ssc_invalid_cmd},
	{0x85,				ssc_invalid_cmd},
	{0x86,				ssc_invalid_cmd},
	{0x87,				ssc_invalid_cmd},
	{SPC3_CMD_READ16,		ssc_read},
	{0x89,				ssc_invalid_cmd},
	{SPC3_CMD_WRITE16,		ssc_write},
	{0x8B,				ssc_invalid_cmd},
	{0x8C,				ssc_invalid_cmd},
	{0x8D,				ssc_invalid_cmd},
	{0x8E,				ssc_invalid_cmd},
	{0x8F,				ssc_invalid_cmd},

	{0x90,				ssc_invalid_cmd},
	{0x91,				ssc_invalid_cmd},
	{0x92,				ssc_invalid_cmd},
	{0x93,				ssc_invalid_cmd},
	{0x94,				ssc_invalid_cmd},
	{0x95,				ssc_invalid_cmd},
	{0x96,				ssc_invalid_cmd},
	{0x97,				ssc_invalid_cmd},
	{0x98,				ssc_invalid_cmd},
	{0x99,				ssc_invalid_cmd},
	{0x9A,				ssc_invalid_cmd},
	{0x9B,				ssc_invalid_cmd},
	{0x9C,				ssc_invalid_cmd},
	{0x9D,				ssc_invalid_cmd},
	{0x9E,				ssc_invalid_cmd},
	{0x9F,				ssc_invalid_cmd},

	{0xA0,				ssc_invalid_cmd},
	{0xA1,				ssc_invalid_cmd},
	{0xA2,				ssc_invalid_cmd},
	{0xA3,				ssc_invalid_cmd},
	{0xA4,				ssc_invalid_cmd},
	{0xA5,				ssc_invalid_cmd},
	{0xA6,				ssc_invalid_cmd},
	{0xA7,				ssc_invalid_cmd},
	{SPC3_CMD_READ12,		ssc_read},
	{0xA9,				ssc_invalid_cmd},
	{SPC3_CMD_WRITE12,		ssc_write},
	{0xAB,				ssc_invalid_cmd},
	{0xAC,				ssc_invalid_cmd},
	{0xAD,				ssc_invalid_cmd},
	{0xAE,				ssc_invalid_cmd},
	{0xAF,				ssc_invalid_cmd},

	{0xB0,				ssc_invalid_cmd},
	{0xB1,				ssc_invalid_cmd},
	{0xB2,				ssc_invalid_cmd},
	{0xB3,				ssc_invalid_cmd},
	{0xB4,				ssc_invalid_cmd},
	{0xB5,				ssc_invalid_cmd},
	{0xB6,				ssc_invalid_cmd},
	{0xB7,				ssc_invalid_cmd},
	{0xB8,				ssc_invalid_cmd},
	{0xB9,				ssc_invalid_cmd},
	{0xBA,				ssc_invalid_cmd},
	{0xBB,				ssc_invalid_cmd},
	{0xBC,				ssc_invalid_cmd},
	{0xBD,				ssc_invalid_cmd},
	{0xBE,				ssc_invalid_cmd},
	{0xBF,				ssc_invalid_cmd},

	{0xC0,				ssc_invalid_cmd},
	{0xC1,				ssc_invalid_cmd},
	{0xC2,				ssc_invalid_cmd},
	{0xC3,				ssc_invalid_cmd},
	{0xC4,				ssc_invalid_cmd},
	{0xC5,				ssc_invalid_cmd},
	{0xC6,				ssc_invalid_cmd},
	{0xC7,				ssc_invalid_cmd},
	{0xC8,				ssc_invalid_cmd},
	{0xC9,				ssc_invalid_cmd},
	{0xCA,				ssc_invalid_cmd},
	{0xCB,				ssc_invalid_cmd},
	{0xCC,				ssc_invalid_cmd},
	{0xCD,				ssc_invalid_cmd},
	{0xCE,				ssc_invalid_cmd},
	{0xCF,				ssc_invalid_cmd},

	{0xD0,				ssc_invalid_cmd},
	{0xD1,				ssc_invalid_cmd},
	{0xD2,				ssc_invalid_cmd},
	{0xD3,				ssc_invalid_cmd},
	{0xD4,				ssc_invalid_cmd},
	{0xD5,				ssc_invalid_cmd},
	{0xD6,				ssc_invalid_cmd},
	{0xD7,				ssc_invalid_cmd},
	{0xD8,				ssc_invalid_cmd},
	{0xD9,				ssc_invalid_cmd},
	{0xDA,				ssc_invalid_cmd},
	{0xDB,				ssc_invalid_cmd},
	{0xDC,				ssc_invalid_cmd},
	{0xDD,				ssc_invalid_cmd},
	{0xDE,				ssc_invalid_cmd},
	{0xDF,				ssc_invalid_cmd},

	{0xE0,				ssc_invalid_cmd},
	{0xE1,				ssc_invalid_cmd},
	{0xE2,				ssc_invalid_cmd},
	{0xE3,				ssc_invalid_cmd},
	{0xE4,				ssc_invalid_cmd},
	{0xE5,				ssc_invalid_cmd},
	{0xE6,				ssc_invalid_cmd},
	{0xE7,				ssc_invalid_cmd},
	{0xE8,				ssc_invalid_cmd},
	{0xE9,				ssc_invalid_cmd},
	{0xEA,				ssc_invalid_cmd},
	{0xEB,				ssc_invalid_cmd},
	{0xEC,				ssc_invalid_cmd},
	{0xED,				ssc_invalid_cmd},
	{0xEE,				ssc_invalid_cmd},
	{0xEF,				ssc_invalid_cmd},

	{0xF0,				ssc_invalid_cmd},
	{0xF1,				ssc_invalid_cmd},
	{0xF2,				ssc_invalid_cmd},
	{0xF3,				ssc_invalid_cmd},
	{0xF4,				ssc_invalid_cmd},
	{0xF5,				ssc_invalid_cmd},
	{0xF6,				ssc_invalid_cmd},
	{0xF7,				ssc_invalid_cmd},
	{0xF8,				ssc_invalid_cmd},
	{0xF9,				ssc_invalid_cmd},
	{0xFA,				ssc_invalid_cmd},
	{0xFB,				ssc_invalid_cmd},
	{0xFC,				ssc_invalid_cmd},
	{0xFD,				ssc_invalid_cmd},
	{0xFE,				ssc_invalid_cmd},
	{0xFF,				ssc_invalid_cmd}
};
