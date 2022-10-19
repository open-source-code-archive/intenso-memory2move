/* 
 * Core Interface for mks32 			
 *
 * Copyright (C) Rainer Link, 2001-2002
 *                            OpenAntiVirus.org <rainer@openantivirus.org>
 *		 Krzysztof Drewicz <hunter@pld.org.pl>, 2002
 *		 Dariusz Markowicz <dm@tenbit.pl>, 2003
 *
 * Credits to W. Richard Stevens - RIP
 * 
 * This software is licensed under the GNU General Public License (GPL)
 * See: http://www.gnu.org/copyleft/gpl.html
 *
*/

#include <includes.h>

#include "vscan-message.h"
#include "vscan-functions.h"
#include "vscan-mksd_core.h"

extern BOOL verbose_file_logging;
extern BOOL send_warning_message;
 
/* initialise socket to mks32
   returns -1 on error or the socket descriptor */
int vscan_mksd_init(void)
{
	if (mksd_connect () < 0) {
		vscan_syslog("ERROR: can not connect to mksd!\n");
		return -1;
	}

	return 0;
}

/*
  If virus is found, logs the filename/virusname into syslog
*/
void vscan_mksd_log_virus(char *infected_file, char *result, char* client_ip)
{
        vscan_syslog_alert("ALERT - Scan result: '%s' infected with virus '%s', client: '%s'", infected_file, result, client_ip);
	if ( send_warning_message )
		vscan_send_warning_message(infected_file, result, client_ip);
        
}



/*
  Scans a file (*FILE*, not a directory - keep that in mind) for a virus
  Expects socket descriptor and file name to scan for
  Returns -2 on a minor error,  -1 on error, 0 if no virus was found, 
  1 if a virus was found 
*/
int vscan_mksd_scanfile(int sockfd, char *scan_file, char* client_ip)
{
	char recv[MAXBUFF],*s;
	char opt[4];

	opt[0] = 'S';
	opt[1] = '\0';

	bzero(recv,sizeof(recv));

	if ( verbose_file_logging )
	        vscan_syslog("INFO: Scanning file : '%s'", scan_file);

	if (mksd_query (scan_file, opt, recv) < 0) {
		if ( verbose_file_logging )
			vscan_syslog("ERROR: file %s not found or not readable or deamon not running", scan_file);
		return -2;
	}
	
	if (memcmp (recv, "VIR ", 4) == 0) {
		/* virus found */
		if ((s = strchr (recv+4, ' ')) != NULL)
			*s = '\0';
		vscan_mksd_log_virus(scan_file, recv+4, client_ip);
		return 1;
	} else if (memcmp (recv, "OK ", 3) == 0) {
   		/* OK */
		if ( verbose_file_logging )
			vscan_syslog("INFO: file %s is clean (%s)", scan_file, recv);
		return 0;
	} else {
		vscan_syslog("ERROR: %s", recv);
	}

	return -1;
}

/*
  close socket
*/
void vscan_mksd_end(int sockfd)
{
	mksd_disconnect ();
}

