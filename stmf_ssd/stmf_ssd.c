/*
 * Copyright 2010 Grigale Ltd. All rights reserved.
 * Use is subject to license terms.
 */

/*
 * COMSTAR Virtual Tape Emulator Driver
 */

#include <sys/types.h>
#include <sys/byteorder.h>
#include <sys/conf.h>
#include <sys/devops.h>
#include <sys/file.h>
#include <sys/modctl.h>
#include <sys/refstr.h>
#include <sys/scsi/scsi.h>
#include <sys/stat.h>
#include <sys/sunldi.h>
#include <sys/stmf.h>
#include <sys/lpif.h>

#include <sys/ddi.h>
#include <sys/sunddi.h>

#include "stmf_ssd.h"
#include "stmf_ssd_ioctl.h"
#include "ssc.h"

#define	STMF_SSDNAME	"stmf_ssd"

static void *stmfssd_statep;

static void
stmf_ssd_lp_cb(stmf_lu_provider_t *lp, int cmd, void *arg, uint32_t flag)
{
}

static int
stmf_ssd_open(dev_t *devp, int flag, int otyp, cred_t *cp)
{
	int		instance;
	stmfssd_state_t	*sp;

	instance = getminor(*devp);

	sp = ddi_get_soft_state(stmfssd_statep, instance);

	if (sp == NULL)
		return (ENXIO);

	if (instance == 0) {
		/* That is a control node */
		return (0);
	} else {
		/* That is LU node, meanwhile not supported */
		return (EINVAL);
	}
}

static int
stmf_ssd_close(dev_t dev, int flag, int otyp, cred_t *cp)
{
	int		instance;
	stmfssd_state_t	*sp;

	instance = getminor(dev);

	sp = ddi_get_soft_state(stmfssd_statep, instance);

	if (sp == NULL)
		return (ENXIO);

	if (instance == 0) {
		return (0);
	} else {
		return (0);
	}
}

/*
 * Create a new SCSI Stream Device instance
 *
 * The order of creation:
 * 1. Allocate new minor number
 * 2. Allocate soft state
 * 3. Register LU with STMF
 * 4. Create minor nodes
 */
#define	BUF_SIZE		128

static int
stmf_ssd_create_dev(stmfssd_state_t *sp, intptr_t arg, int mode, cred_t *crp)
{
	char			buf[BUF_SIZE];
	stmfssd_cmd_t		cmd;
	dev_t			dev;
	minor_t			minor;
	stmfssd_state_t		*nsp;
	stmfssd_create_dev_cmd_t	*cdcp;

	/* Bring the command in */
	if (ddi_copyin((void*)arg, &cmd, sizeof (cmd), mode) != 0) {
		return (EFAULT);
	}

	if (cmd.version != STMFSSD_ABI_VERSION_1) {
		return (EINVAL);
	}

	if (cmd.len > BUF_SIZE) {
		cmn_err(CE_WARN, "The CREATE_DEV command payload is too big");
		return (ENOMEM);
	}

	if (ddi_copyin((void*)(arg + sizeof (cmd)), buf, cmd.len, mode) != 0) {
		return (EFAULT);
	}

	cdcp = (stmfssd_create_dev_cmd_t *)buf;

	/* Search for first available minor number */
	for (minor = 1; ddi_get_soft_state(stmfssd_statep, minor) != NULL; minor++)
		;

	dev = makedevice(ddi_driver_major(sp->dip), minor);

	/* Allocate soft state for internal book-keeping */
	if (ddi_soft_state_zalloc(stmfssd_statep, minor) != DDI_SUCCESS) {
		cmn_err(CE_WARN, "Cannot allocate soft state for new device");
		return (ENOMEM);
	}

	nsp = ddi_get_soft_state(stmfssd_statep, minor);

	/* Copy the LU provider info */
	nsp->lpp = sp->lpp;

	/* Copy device INQUIRY identification */
	bcopy(cdcp->vendor, nsp->vid, 8);
	bcopy(cdcp->product, nsp->pid, 16);
	bcopy(cdcp->fwrev, nsp->fwrev, 4);

	if (lu_create(nsp) == STMF_FAILURE) {
		ddi_soft_state_free(stmfssd_statep, minor);
		return (ENOMEM);
	}

	(void) snprintf(buf, sizeof (buf), "%d", minor);

	if (ddi_create_minor_node(sp->dip, buf, S_IFCHR, minor, DDI_PSEUDO,
	    NULL) != DDI_SUCCESS) {
		lu_remove(nsp);
		ddi_soft_state_free(stmfssd_statep, minor);
		return (ENXIO);
	}

	return (0);
}

static int
stmf_ssd_remove_dev(stmfssd_state_t *sp, intptr_t arg, int mode, cred_t *crp)
{
	stmfssd_cmd_t		cmd;
	stmfssd_remove_dev_cmd_t	dev;
	stmfssd_state_t		*nsp;
	char			buf[BUF_SIZE];

	/* Bring the command in */
	if (ddi_copyin((void*)arg, &cmd, sizeof (cmd), mode) != 0) {
		return (EFAULT);
	}

	if (cmd.version != STMFSSD_ABI_VERSION_1) {
		return (EINVAL);
	}

	if (cmd.len != sizeof (dev)) {
		return (EINVAL);
	}

	if (ddi_copyin((void*)(arg + sizeof (cmd)), &dev, cmd.len, mode) != 0) {
		return (EFAULT);
	}

	nsp = ddi_get_soft_state(stmfssd_statep, dev.minor);
	if (nsp == NULL) {
		return (EINVAL);
	}

	(void) snprintf(buf, sizeof (buf), "%d", dev.minor);
	ddi_remove_minor_node(sp->dip, buf);
	(void) snprintf(buf, sizeof (buf), "%d,raw", dev.minor);
	ddi_remove_minor_node(sp->dip, buf);
	lu_remove(nsp);
	refstr_rele(nsp->dev);
	ddi_soft_state_free(stmfssd_statep, dev.minor);
	return (0);
}

static int
stmf_ssd_admin_ioctl(stmfssd_state_t *sp, int cmd, intptr_t arg, int mode,
    cred_t *crp, int *rvp)
{
	int		rc = 0;
	stmfssd_reply_t	reply;

	reply.version = STMFSSD_ABI_VERSION_1;
	reply.status = 0;
	reply.len = 0;

	switch (cmd) {
	case STMFSSD_GET_VERSION:
		if (ddi_copyout(&reply, (void*)arg,
		    sizeof (reply), mode) != 0) {
			rc = EFAULT;
		}
		break;
	case STMFSSD_LIST_DEV:
		reply.len = 0;
		break;
	case STMFSSD_CREATE_DEV:
		rc = stmf_ssd_create_dev(sp, arg, mode, crp);
		break;
	case STMFSSD_REMOVE_DEV:
		rc = stmf_ssd_remove_dev(sp, arg, mode, crp);
		break;
	default:
		rc = EINVAL;
	}

	return (rc);
}

static int
stmf_ssd_lu_ioctl(stmfssd_state_t *sp, int cmd, intptr_t arg, int mode,
    cred_t *cp, int *rvp)
{
	return (0);
}

static int
stmf_ssd_ioctl(dev_t dev, int cmd, intptr_t arg, int mode, cred_t *cp, int *rvp)
{
	int		instance;
	stmfssd_state_t	*sp;

	instance = getminor(dev);

	sp = ddi_get_soft_state(stmfssd_statep, instance);

	if (sp == NULL)
		return (ENXIO);

	if (instance == 0) {
		return (stmf_ssd_admin_ioctl(sp, cmd, arg, mode, cp, rvp));
	} else {
		return (stmf_ssd_lu_ioctl(sp, cmd, arg, mode, cp, rvp));
	}
}

static struct cb_ops stmf_ssd_cb_ops = {
	.cb_open	= stmf_ssd_open,
	.cb_close	= stmf_ssd_close,
	.cb_strategy	= nodev,
	.cb_print	= nodev,
	.cb_dump	= nodev,
	.cb_read	= nodev,
	.cb_write	= nodev,
	.cb_ioctl	= stmf_ssd_ioctl,
	.cb_devmap	= nodev,
	.cb_mmap	= nodev,
	.cb_segmap	= nodev,
	.cb_chpoll	= nochpoll,
	.cb_prop_op	= ddi_prop_op,
	.cb_str		= NULL,
	.cb_flag	= D_NEW | D_MP | D_64BIT,
	.cb_rev		= CB_REV,
	.cb_aread	= nodev,
	.cb_awrite	= nodev
};

static int
stmf_ssd_getinfo(dev_info_t *dip, ddi_info_cmd_t cmd, void *arg, void **rp)
{
	int		instance;
	stmfssd_state_t	*sp;
	int		rc;

	instance = getminor((dev_t)arg);

	switch (cmd) {
	case DDI_INFO_DEVT2DEVINFO:
		sp = ddi_get_soft_state(stmfssd_statep, instance);
		if (sp != NULL) {
			*rp = sp->dip;
			rc = DDI_SUCCESS;
		} else {
			*rp = NULL;
			rc = DDI_FAILURE;
		}
		break;
	case DDI_INFO_DEVT2INSTANCE:
		*rp = (void *)(uintptr_t)instance;
		rc = DDI_SUCCESS;
		break;
	default:
		rc = DDI_FAILURE;
	}

	return (rc);
}

static int
stmf_ssd_attach(dev_info_t *dip, ddi_attach_cmd_t cmd)
{
	stmfssd_state_t	*sp;
	int		instance;

	instance = ddi_get_instance(dip);

	ASSERT(instance == 0);

	switch (cmd) {
	case DDI_ATTACH:
		break;
	case DDI_RESUME:
	default:
		return (DDI_FAILURE);
	}

	if (ddi_soft_state_zalloc(stmfssd_statep, instance) != DDI_SUCCESS)
		return (DDI_FAILURE);

	sp = ddi_get_soft_state(stmfssd_statep, instance);

	sp->dip = dip;

	sp->lpp = stmf_alloc(STMF_STRUCT_LU_PROVIDER, 0, 0);

	sp->lpp->lp_lpif_rev = LPIF_REV_1;
	sp->lpp->lp_instance = 0;
	sp->lpp->lp_name = STMF_SSDNAME;
	sp->lpp->lp_cb = stmf_ssd_lp_cb;

	if (stmf_register_lu_provider(sp->lpp) != STMF_SUCCESS) {
		stmf_free(sp->lpp);
		ddi_soft_state_free(stmfssd_statep, instance);
		return (DDI_FAILURE);
	}

	if (ddi_create_minor_node(dip, "admin", S_IFCHR,
	    instance, DDI_PSEUDO, 0) != DDI_SUCCESS) {
		if (stmf_deregister_lu_provider(sp->lpp) != STMF_SUCCESS) {
			/*
			 * The failure at this point is most unexpected and we
			 * have nothing else to do anyhow, so just continue
			 */
			cmn_err(CE_WARN, "Failed to deregister LU provider "
			    "during attach failure; please reboot");
		}
		stmf_free(sp->lpp);
		ddi_soft_state_free(stmfssd_statep, instance);
		return (DDI_FAILURE);
	}

	ddi_report_dev(dip);

	return (DDI_SUCCESS);
}

static int
stmf_ssd_detach(dev_info_t *dip, ddi_detach_cmd_t cmd)
{
	stmfssd_state_t	*sp;
	int		instance;

	instance = ddi_get_instance(dip);

	VERIFY(instance == 0);

	switch (cmd) {
	case DDI_DETACH:
		break;
	case DDI_SUSPEND:
	default:
		return (DDI_FAILURE);
	}

	sp = ddi_get_soft_state(stmfssd_statep, instance);

	/* XXX Make sure there are no LU instances anymore */
	/*
	 * I want to do it first, since if it fails we are leaving the instance
	 * in the unknown state
	 */
	if (stmf_deregister_lu_provider(sp->lpp) != STMF_SUCCESS)
		return (DDI_FAILURE);

	ddi_remove_minor_node(dip, 0);

	stmf_free(sp->lpp);

	ddi_soft_state_free(stmfssd_statep, instance);

	return (DDI_SUCCESS);
}

static struct dev_ops stmf_ssd_dev_ops = {
	.devo_rev	= DEVO_REV,
	.devo_refcnt	= 0,
	.devo_getinfo	= stmf_ssd_getinfo,
	.devo_identify	= nulldev,
	.devo_probe	= nulldev,
	.devo_attach	= stmf_ssd_attach,
	.devo_detach	= stmf_ssd_detach,
	.devo_reset	= nodev,
	.devo_cb_ops	= &stmf_ssd_cb_ops,
	.devo_bus_ops	= NULL,
	.devo_power	= NULL,
	.devo_quiesce	= ddi_quiesce_not_supported
};

static struct modldrv stmf_ssd_modldrv = {
	.drv_modops	= &mod_driverops,
	.drv_linkinfo	= "COMSTAR SSD v1",
	.drv_dev_ops	= &stmf_ssd_dev_ops
};

static struct modlinkage stmf_ssd_modlinkage = {
	.ml_rev		= MODREV_1,
	.ml_linkage	= {&stmf_ssd_modldrv, NULL, NULL, NULL}
};

int
_init(void)
{
	int rc;

	rc = ddi_soft_state_init(&stmfssd_statep, sizeof (stmfssd_state_t), 0);
	if (rc != 0)
		return (rc);
	rc = mod_install(&stmf_ssd_modlinkage);
	if (rc != 0)
		ddi_soft_state_fini(&stmfssd_statep);
	return (rc);
}

int
_info(struct modinfo *mip)
{
	return (mod_info(&stmf_ssd_modlinkage, mip));
}

int
_fini(void)
{
	int rc;

	rc = mod_remove(&stmf_ssd_modlinkage);
	if (rc == 0)
		ddi_soft_state_fini(&stmfssd_statep);
	return (rc);
}
