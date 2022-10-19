/*
 * libmksd.c, ver. 1.04
 * copyright (c) MkS Sp. z o.o. 2002,2003
 * license: LGPL (see COPYING.LIB for details)
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/param.h>

#include "libmksd.h"

#define MAXTRIES 5


static char *cwd = NULL;
static int cwdlen;
static int fd = -1;


static int do_writev (struct iovec *iov, int n, int len)
{
	int i;
	
	do {
		do
			i = writev (fd, iov, n);
		while ((i < 0) && (errno == EINTR));
		if (i <= 0)
			return -1;
		
		if ((len -= i) > 0)
			do
				if (i < iov->iov_len) {
					iov->iov_len -= i;
					iov->iov_base = (void *)
					  ((char *)iov->iov_base + i);
					i = 0;
				} else {
					i -= iov->iov_len;
					iov ++;
					n --;
				}
			while (i > 0);
	} while (len > 0);
	
	return 0;
}

static int read_line (char *s)
{
	int i;
	
	do {
		do
			i = read (fd, s, 4096);
		while ((i < 0) && (errno == EINTR));
		if (i <= 0)
			return -1;
		
		for ( ; (i > 0) && (*s != '\n'); i--, s++);
	} while (i == 0);
	
	*s = '\0';
	
	return 0;
}

static int get_cwd ()
{
	if ((cwd = getcwd (NULL, 0)) == NULL)
		return -1;
	
	cwdlen = strlen (cwd);
	cwd [cwdlen++] = '/';
	
	return 0;
}


int mksd_connect ()
{
	struct sockaddr_un serv;
	unsigned sun_len;
	int i, cnt = 0;
	struct timespec ts = {1, 0};
	
	if ((fd = socket (PF_UNIX, SOCK_STREAM, 0)) < 0)
		return -1;
	
	strcpy (serv.sun_path, "/var/run/mksd/socket");
	sun_len = SUN_LEN (&serv);
#ifdef _44BSD_
	serv.sun_len = sun_len;
#endif
	serv.sun_family = AF_UNIX;
	
	do {
		if (cnt > 0)
			nanosleep (&ts, NULL);
		i = connect (fd, (struct sockaddr *)&serv, sun_len);
	} while ((i < 0) && (errno == EAGAIN) && (++cnt < MAXTRIES));
	if (i < 0)
		return -1;
	
	return fd;
}

int mksd_query (const char *que, const char *prfx, char *ans)
{
	struct iovec iov [4];
	char enter = '\n';
	int len, plen, n;
	
	for (len = 0; que [len] != '\0'; len++)
		if (que [len] == '\n')
			return -1;
	if (len > MAXPATHLEN)
		return -1;
	if ((plen = (prfx ? strlen (prfx) : 0)) > 16)
		return -1;
	
	iov[3].iov_base = (void *)&enter;
	iov[3].iov_len = 1;
	
	if (*que == '/') {
		iov[2].iov_base = (void *)que;
		iov[2].iov_len = len;
		n = 2;
	} else {
		if ((que[0] == '.') && (que[1] == '/')) {
			iov[2].iov_base = (void *)(que+2);
			len -= 2;
		} else
			iov[2].iov_base = (void *)que;
		iov[2].iov_len = len;
		if (cwd == NULL)
			if (get_cwd () != 0)
				return -1;
		iov[1].iov_base = (void *)cwd;
		iov[1].iov_len = cwdlen;
		n = 3;
		len += cwdlen;
	}
	
	if (plen) {
		n ++;
		len += plen;
		iov[4-n].iov_base = (void *)prfx;
		iov[4-n].iov_len = plen;
	}
		
	if (do_writev (iov + (4-n), n, ++len) < 0)
		return -1;
	
	return read_line (ans);
}

void mksd_disconnect ()
{
	close (fd);
	fd = -1;
	
	free (cwd);
	cwd = NULL;
}
