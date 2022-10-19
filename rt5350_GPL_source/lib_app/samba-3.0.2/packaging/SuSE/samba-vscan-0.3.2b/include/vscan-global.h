#ifndef __VSCAN_GLOBAL_H_
#define __VSCAN_GLOBAL_H_


#include "config.h"

#include <sys/types.h>

#ifdef TIME_WITH_SYS_TIME
 #include <sys/time.h>
 #include <time.h>
#else
 #ifdef HAVE_SYS_TIME_H
  #include <sys/time.h>
 #else
  #include <time.h>
 #endif
#endif

#ifdef HAVE_UTIME_H
 #include <utime.h>
#endif

#ifdef HAVE_DIRENT_H
 #include <dirent.h>
#endif

#ifdef HAVE_FCNTL_H
 #include <fcntl.h>
#else
 #ifdef HAVE_SYS_FCNTL_H
  #include <sys/fcntl.h>
 #endif
#endif

#include <stdio.h>

#ifdef HAVE_STDLIB_H
 #include <stdlib.h>
#endif

#include <syslog.h>

#ifdef HAVE_STDARG_H
 #include <stdarg.h>
#else
 #include <varargs.h>
#endif


#ifdef HAVE_UNISTD_H
 #include <unistd.h>
#endif

#ifdef HAVE_STRING_H
 #include <string.h>
#endif

#ifdef HAVE_STRINGS_H
 #include <strings.h>
#endif

#include <errno.h>
#include <pwd.h>
#include <sys/stat.h>


#ifndef bool 
 #ifndef BOOL_DEFINED
   typedef int bool;
 #endif
#endif 

#ifndef FALSE
# define FALSE	0
#endif /* ! FALSE */
#ifndef TRUE
# define TRUE	1
#endif /* ! TRUE */

/* Configuration Section :-) */

/* which samba version is this VFS module compiled for:
 * Set SAMBA_VERSION_MAJOR to 3 for Samba 3.0 or
 * to 2 for Samba 2.2.x
 * Set SAMBA_VERSION_MINOR to 4 for Samba >=2.2.4 
 * Set it to 2 if you're using Samba 2.2.2/2.2.3
 * Set it to 1 if you're using Samba 2.2.1[a] or 0 for Samba 2.2.0[a] 
 * If SAMBA_VERSION_MAJOR is set to 3, SAMBA_VERSION_MINOR
 * is ignored!
 *
 * Per default, Samba >=2.2.4 is assumed!
*/

#ifndef SAMBA_VERSION_MAJOR
# define SAMBA_VERSION_MAJOR 2 
#endif

#ifndef SAMBA_VERSION_MINOR
# define SAMBA_VERSION_MINOR 4
#endif 

#ifndef SYSLOG_FACILITY
#define SYSLOG_FACILITY   LOG_USER
#endif

#ifndef SYSLOG_PRIORITY
#define SYSLOG_PRIORITY   LOG_NOTICE
#endif

/* virus messages will be logged as SYSLOG_PRIORITY_ALERT */
#ifndef SYSLOG_PRIORITY_ALERT
#define SYSLOG_PRIORITY_ALERT   LOG_ERR
#endif






#endif /* __VSCAN_GLOBAL_H */
