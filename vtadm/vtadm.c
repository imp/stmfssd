/*
 * Copyright 2010 Grigale Ltd. All right reserved.
 * Use is subject to license terms.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define	STMFSSD_CTL_DEV	"/dev/stmf_ssdctl"

typedef struct {
	int	fd;
} ctldev_t;

static ctldev_t ctldev;

bool
openctldev()
{
	ctldev.fd = open(STMFSSD_CTL_DEV, O_RDWR);
	if (ctldev.fd == -1) {
		/* No dice */
		return (false);
	}

	return (true);
}

void
closectldev()
{
	close(ctldev.fd);
}

int
parseopt(int argc, char **argv)
{
	if (argc == 1) {
		return (0);
	}
}

int
main(int argc, char **argv)
{
	printf("vtadm is not implemented yet!\n");

	parseopt(argc, argv);

	if (!openctldev()) {
		printf("Cannot open stmf_ssd control device\n");
		exit(EXIT_FAILURE);
	}

	closectldev();

	return (0);
}
