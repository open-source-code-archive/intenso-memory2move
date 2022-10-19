/*
 * $Id: vscan-oav_core.c,v 1.17 2003/03/17 19:19:07 reniar Exp $
 * 
 * Core Interface for OpenAntiVirus ScannerDaemon			
 *
 * Copyright (C) Rainer Link, 2001-2002
 *               OpenAntiVirus.org <rainer@openantivirus.org>
 *
 * Credits to W. Richard Stevens - RIP
 * 
 * This software is licensed under the GNU General Public License (GPL)
 * See: http://www.gnu.org/copyleft/gpl.html
 *
*/

#include <includes.h>

#include "vscan-oav_core.h"
#include "vscan-message.h"
#include "vscan-functions.h"

/* hum, global vars ... */
extern BOOL verbose_file_logging;
extern BOOL send_warning_message;
extern fstring  oav_ip;
extern unsigned short int oav_port;


/* initialise socket to ScannerDaemon
   returns -1 on error or the socket descriptor */
int vscan_oav_init(void)
{

	int sockfd;
	struct sockaddr_in servaddr;

	/* create socket */
        if (( sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
               vscan_syslog("ERROR: can not create socket!\n");
               return -1; 
        }

	bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(oav_port);

	/* hm, inet_pton may not exist on all systems - FIXME ! */
        if ( inet_pton(AF_INET, oav_ip, &servaddr.sin_addr) <= 0 ) {
                vscan_syslog("ERROR: inet_pton failed!\n");
                return -1;
	}

	/* connect to socket */
	if ( connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
        {
                vscan_syslog("ERROR: can not connect to ScannerDaemon!\n");
                return -1;
        }


	return sockfd;

}

/*
  If virus is found, logs the filename/virusname into syslog
*/
void vscan_oav_log_virus(char *infected_file, char *result, char* client_ip)
{
        char *str;
        size_t len;

        /* remove "FOUND: " from the result string to get only the virus name */

	len = strlen(result);
        /* sanity check ... */
        if ( len < 7 ) {
            /* hum, sth went wrong ... */
            vscan_syslog_alert("ALERT - Scan result: '%s' infected with virus 'UNKOWN', client: '%s'", infected_file, client_ip);
	    if ( send_warning_message )
		vscan_send_warning_message(infected_file, "UNKNOWN", client_ip);

        } else {
            str = result;
            str+= 7;
            vscan_syslog_alert("ALERT - Scan result: '%s' infected with virus '%s', client: '%s'", infected_file, str, client_ip);
	    if ( send_warning_message )
		vscan_send_warning_message(infected_file, str, client_ip);
        }
}



/*
  Scans a file (*FILE*, not a directory - keep that in mind) for a virus
  Expects socket descriptor and file name to scan for
  Returns -2 on a minor error, -1 on error, 0 if no virus was found, 
  1 if a virus was found 
*/
int vscan_oav_scanfile(int sockfd, char *scan_file, char* client_ip)
{
	size_t len;
	char recvline[MAXLINE + 1];
	pstring oavCommand;

	FILE *fpin, *fpout;

	fpin = fdopen(sockfd, "r");
	if ( fpin == NULL ) {
		vscan_syslog("ERROR: can not open stream for reading - %s", strerror(errno));
		return -1;
	}

	fpout = fdopen(sockfd, "w");
	if ( fpout == NULL ) {
		vscan_syslog("ERROR: can not open stream for writing - %s", strerror(errno));
		return -1;
	}

	if ( verbose_file_logging )
	        vscan_syslog("INFO: Scanning file : '%s'", scan_file);

	/* ScannerDaemon expects "SCAN <filename>\n" */
	/* what about if the <filename> itself contains '\n'? */
        pstrcpy(oavCommand, "SCAN ");
        pstrcat(oavCommand, scan_file);
        pstrcat(oavCommand, "\n");

	/* NOTE: what happens if scan_file is very long? */

	/* write to socket-stream */
	if ( fputs(oavCommand, fpout) == EOF) {
		vscan_syslog("ERROR: can not send file name to ScannerDaemon!");
		return -1;
	}

	/* hum, instead of flush()ing, use setvbuf to set line-based buffering? */
	if ( fflush(fpout) == EOF ) {
		vscan_syslog("ERROR: can not flush output stream - %s", strerror(errno));
		/* FIXME: shouldn't we return -1 here? */
	}

	/* read from socket-stream */
	if ( fgets(recvline, MAXLINE, fpin) == NULL ) {
		vscan_syslog("ERROR: can not get result from ScannerDaemon!");
		return -1;
	}

	/* Response from ScannerDaemon is either
	   - OK                  file scanned and not infected
	   - FOUND: <virusname>  file scanned and infected
	   - ERROR		 sth went wrong, i.e. file not found
	*/
	if ( strncmp("FOUND", recvline, 5) == 0   ) {
		/* virus found */
		vscan_oav_log_virus(scan_file, recvline, client_ip);
		return 1;
	} else if ( strncmp("ERROR", recvline, 5) == 0  ) {
		/* ERROR */
		if ( verbose_file_logging )
			vscan_syslog("ERROR: file %s not found, not readable or an error occured", scan_file);
		return -2;
	} else if ( strncmp("OK", recvline, 2) == 0 ) {
		/* OK */
		if ( verbose_file_logging )
			vscan_syslog("INFO: file %s is clean", scan_file);
        } else {
		/* unknown response */
		vscan_syslog("ERROR: unknown response from ScannerDaemon while scanning %s!", scan_file);
		/* FIXME: should we really mark this as a minor error? */
		return -2;
	}	

	return 0;
	
}

/*
  close socket
*/
void vscan_oav_end(int sockfd)
{
	/* sockfd == -1 indicates an error while connecting to socket */
	if ( sockfd >= 0 ) {
		close(sockfd);
	}

}
