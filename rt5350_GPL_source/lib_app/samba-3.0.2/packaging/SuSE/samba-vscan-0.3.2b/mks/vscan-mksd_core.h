#ifndef __VSCAN_MKSD_CORE_H_
#define __VSCAN_MKSD_CORE_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "vscan-mksd.h"
#include "mks.h"
#ifdef _USE_INCL_MKSD_LIB
 #include "libmksd/libmksd.h"
#else
 #include <libmksd.h>
#endif

#endif /* __VSCAN_MKSD_CORE_H */
