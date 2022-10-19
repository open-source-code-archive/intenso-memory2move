/* 
 * $Id: vscan-quarantine.c,v 1.3 2002/12/03 17:06:01 reniar Exp $
 *
 * Provides functions for quarantining or removing an infected file
 *
 * Copyright (C) Kurt Huwig, 2002
 *		 OpenAntiVirus.org <kurt@openantivirus.org>
 *		 Rainer Link, 2002
 *		 OpenAntiVirus.org <rainer@openantivirus.org>
 *
 * This software is licensed under the GNU General Public License (GPL)
 * See: http://www.gnu.org/copyleft/gpl.html
 *
*/

#include <includes.h>
#include <vfs.h>

#include "vscan-functions.h"
#include "vscan-quarantine.h"

/*
  deletes the infected file
*/
 

int vscan_delete_virus(struct vfs_ops *ops, struct connection_struct *conn, char *virus_file) {
	int rc = ops->unlink(conn, virus_file);

	if (rc) {
		vscan_syslog_alert("ERROR: removing file '%s' failed, reason: %s", virus_file, strerror(errno));
		return rc;
	}
	vscan_syslog("INFO: file '%s' removed successfully", virus_file);
	return 0;
}

/*
  moves the infected file to quarantine
*/
int vscan_quarantine_virus(struct vfs_ops *ops, struct connection_struct *conn, char *virus_file, char *q_dir, char *q_prefix) {
	int rc;
	/* FIXME: tempnam should be avoided */
	char *q_file = tempnam(q_dir, q_prefix);

	if (q_file == NULL) {
		vscan_syslog_alert("ERROR: cannot create unique quarantine filename. Probably a permission problem with directory %s", q_dir);
		return -1;
	}
	rc = ops->rename(conn, virus_file, q_file);
	if (rc) {
		vscan_syslog_alert("ERROR: quarantining file '%s' to '%s' failed, reason: %s; removing file", virus_file, q_file, strerror(errno));

/* FIXME: we should not remove any file per default. An infected word document 
   may contain important data. Add "delete file on quarantine failure" for
   the next version.
		return vscan_delete_virus(ops, conn, virus_file);
*/
		return -1;
	}
	vscan_syslog("INFO: quarantining file '%s' to '%s' was successful", virus_file, q_file);
	return 0;
}
 

int vscan_do_infected_file_action(struct vfs_ops *ops, struct connection_struct *conn, char *virus_file, char *q_dir, char *q_prefix, enum infected_file_action_enum infected_file_action) {

	int rc;

	switch (infected_file_action) {
	case INFECTED_QUARANTINE:
		rc = vscan_quarantine_virus(ops, conn, virus_file, q_dir, q_prefix);
		break;
	case INFECTED_DELETE:
		rc = vscan_delete_virus(ops, conn, virus_file);
		break;
	case INFECTED_DO_NOTHING:
		rc = 0;
		break;
	default:
		vscan_syslog_alert("unknown infected file action %d!", infected_file_action);
		break; /* FIXME: do we really need a break here?!? */
	}

	return rc;
}
