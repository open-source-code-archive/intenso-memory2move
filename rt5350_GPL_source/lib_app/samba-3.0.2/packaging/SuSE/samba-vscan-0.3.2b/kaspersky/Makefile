#
# Makefile for vscan-kavp, part of samba-vscan
#
# Ries van Twisk (vscan@rvt.dds.nl) 2002/01/22
#
# Make file is taken from the trend makefile in the samba-vscan package
#

# Variables

#USE_DEBUG=1
#USE_KAVPSHAREDLIB=1

CC = gcc
LIBTOOL = libtool

SAMBA_SRC = ../../../../source
SAMBA_INCL = ../../../../source/include
UBIQX_SRC = ../../../../source/ubiqx
SMBWR_SRC = ../../../../source/smbwrapper
SMBVS_INCL = ../include
SMBVS_GLB = ../global
VFS_OBJS = vscan-kavp.so

ifdef USE_DEBUG
CFLAGS = -I$(SAMBA_SRC) -I$(SAMBA_INCL) -I$(UBIQX_SRC) -I$(SMBWR_SRC) -I$(SMBVS_INCL) -Wall -g -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC 
else
CFLAGS = -I$(SAMBA_SRC) -I$(SAMBA_INCL) -I$(UBIQX_SRC) -I$(SMBWR_SRC) -I$(SMBVS_INCL) -Wall -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC
endif

ifndef USE_KAVPSHAREDLIB
LIBS = -L/usr/lib /usr/lib/kavdclib.so
OBJS = vscan-functions.lo vscan-message.lo vscan-quarantine.lo vscan-fileaccesslog.lo vscan-kavp.lo vscan-kavp_core.lo kavclient.lo
SOURCES = $(SMBVS_GLB)/vscan-functions.c $(SMBVS_GLB)/vscan-message.c $(SMBVS_GLB)/vscan-quarantine.c $(SMBVS_GLB)/vscan-fileaccesslog.c vscan-kavp.c vscan-kavp_core.c vscan-kavp.h vscan-kavp_core.h kavclient.h kavclient.c helpers.c helpers.h 
else
LIBS = 
OBJS = vscan-functions.lo vscan-message.lo vscan-quarantine.lo vscan-fileaccesslog.lo vscan-kavp.lo vscan-kavp_core.lo 
SOURCES = $(SMBVS_GLB)/vscan-functions.c $(SMBVS_GLB)/vscan-message.c $(SMBVS_GLB)/vscan-quarantine.c $(SMBVS_GLB)/vscan-fileaccesslog.c vscan-kavp.c vscan-kavp_core.c vscan-kavp.h vscan-kavp_core.h kavclient.h
endif

# Default target

all: /usr/lib/kavdclib.so $(VFS_OBJS)

/usr/lib/kavdclib.so:
	echo "Error: /usr/lib/kavdclib.so must be installed first (KAV3)"
	echo "If you have KAV4, you're using the wrong Makefile"
	exit 1

vscan-kavp.so: $(SOURCES)
ifndef USE_KAVPSHAREDLIB
	$(LIBTOOL) $(CC) $(CFLAGS) -c kavclient.c
endif
	$(LIBTOOL) $(CC) $(CFLAGS) -c helpers.c
	$(LIBTOOL) $(CC) $(CFLAGS) -c $(SMBVS_GLB)/vscan-functions.c
	$(LIBTOOL) $(CC) $(CFLAGS) -c $(SMBVS_GLB)/vscan-message.c
	$(LIBTOOL) $(CC) $(CFLAGS) -c $(SMBVS_GLB)/vscan-quarantine.c
	$(LIBTOOL) $(CC) $(CFLAGS) -c $(SMBVS_GLB)/vscan-fileaccesslog.c
	$(LIBTOOL) $(CC) $(CFLAGS) -c vscan-kavp.c
	$(LIBTOOL) $(CC) $(CFLAGS) -c vscan-kavp_core.c
	$(LIBTOOL) $(CC) $(CFLAGS) -shared $(LDFLAGS) $(LIBS) $(OBJS) -o vscan-kavp.so
	rm -f *.lo

# Misc targets
clean:
	rm -rf .libs
	rm -f core *~ *% *.bak *.so *.lo *.o

install:
	cp vscan-kavp.so /usr/lib/samba/vfs/vscan-kavp.so

reload:
	skill -HUP smbd
	skill -HUP nmbd
	
