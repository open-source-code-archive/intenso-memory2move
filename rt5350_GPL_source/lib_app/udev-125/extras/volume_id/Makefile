# Makefile for udev extra invoked from the udev main Makefile
#
# Copyright (C) 2004-2006 Kay Sievers <kay.sievers@vrfy.org>
#
# Released under the GNU General Public License, version 2.
#

PROG = vol_id
GEN_HEADERS =
MAN_PAGES = vol_id.8

prefix =
etcdir =	${prefix}/etc
sbindir =	${prefix}/sbin
usrbindir =	${prefix}/usr/bin
usrsbindir =	${prefix}/usr/sbin
libudevdir =	${prefix}/lib/udev
mandir =	${prefix}/usr/share/man
configdir =	${etcdir}/udev/

INSTALL = install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA  = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}

all: lib $(PROG) $(MAN_PAGES)
.PHONY: all
.DEFAULT: all

.SUFFIXES:

%.o: %.c $(GEN_HEADERS)
	$(E) "  CC      " $@
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

lib:
	$(Q) $(MAKE) -C $@
.PHONY: lib

lib/libvolume_id.a: lib

$(PROG): %: $(HEADERS) %.o lib/libvolume_id.a
	$(E) "  LD      " $@
ifeq ($(strip $(VOLUME_ID_STATIC)),true)
	$(Q) $(LD) $(LDFLAGS) -o $@ $@.o $(LIBUDEV) lib/libvolume_id.a $(LIB_OBJS)
else
	$(Q) $(LD) $(LDFLAGS) -o $@ $@.o $(LIBUDEV) -Llib -lvolume_id $(LIB_OBJS)
endif

# man pages
%.8: %.xml
	$(E) "  XMLTO   " $@
	$(Q) xmlto man $?
.PRECIOUS: %.8

clean:
	$(E) "  CLEAN   "
	$(Q) rm -f $(PROG) $(OBJS) $(GEN_HEADERS)
	$(Q) $(MAKE) -C lib clean
.PHONY: clean

install-bin: all
	$(INSTALL) -d $(DESTDIR)$(libudevdir)
	$(INSTALL_PROGRAM) $(PROG) $(DESTDIR)$(libudevdir)/$(PROG)
	$(MAKE) -C lib install
.PHONY: install-bin

uninstall-bin:
	- rm $(DESTDIR)$(libudevdir)/$(PROG)
	$(MAKE) -C lib uninstall
.PHONY: uninstall-bin

install-man:
	$(INSTALL) -d $(DESTDIR)$(mandir)/man8
	$(INSTALL_DATA) $(PROG).8 $(DESTDIR)$(mandir)/man8/$(PROG).8
.PHONY: install-man

uninstall-man:
	-rm -f $(DESTDIR)$(mandir)/man8/$(PROG).8
.PHONY: uninstall-man

install-config:
	@echo "no config file to install"
.PHONY: install-config
