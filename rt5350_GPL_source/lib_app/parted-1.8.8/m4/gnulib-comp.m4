# -*- buffer-read-only: t -*- vi: set ro:
# DO NOT EDIT! GENERATED AUTOMATICALLY!
# DO NOT EDIT! GENERATED AUTOMATICALLY!
# Copyright (C) 2004-2007 Free Software Foundation, Inc.
#
# This file is free software, distributed under the terms of the GNU
# General Public License.  As a special exception to the GNU General
# Public License, this file may be distributed as part of a program
# that contains a configuration script generated by Autoconf, under
# the same distribution terms as the rest of that program.
#
# Generated by gnulib-tool.
#
# This file represents the compiled summary of the specification in
# gnulib-cache.m4. It lists the computed macro invocations that need
# to be invoked from configure.ac.
# In projects using CVS, this file can be treated like other built files.


# This macro should be invoked from ./configure.ac, in the section
# "Checks for programs", right after AC_PROG_CC, and certainly before
# any checks for libraries, header files, types and library functions.
AC_DEFUN([gl_EARLY],
[
  m4_pattern_forbid([^gl_[A-Z]])dnl the gnulib macro namespace
  m4_pattern_allow([^gl_ES$])dnl a valid locale name
  m4_pattern_allow([^gl_LIBOBJS$])dnl a variable
  m4_pattern_allow([^gl_LTLIBOBJS$])dnl a variable
  AC_REQUIRE([AC_PROG_RANLIB])
  AC_REQUIRE([AC_GNU_SOURCE])
  AC_REQUIRE([gl_USE_SYSTEM_EXTENSIONS])
  dnl Some compilers (e.g., AIX 5.3 cc) need to be in c99 mode
  dnl for the builtin va_copy to work.  With Autoconf 2.60 or later,
  dnl AC_PROG_CC_STDC arranges for this.  With older Autoconf AC_PROG_CC_STDC
  dnl shouldn't hurt, though installers are on their own to set c99 mode.
  AC_REQUIRE([AC_PROG_CC_STDC])
])

# This macro should be invoked from ./configure.ac, in the section
# "Check for header files, types and library functions".
AC_DEFUN([gl_INIT],
[
  m4_pushdef([AC_LIBOBJ], m4_defn([gl_LIBOBJ]))
  m4_pushdef([AC_REPLACE_FUNCS], m4_defn([gl_REPLACE_FUNCS]))
  m4_pushdef([AC_LIBSOURCES], m4_defn([gl_LIBSOURCES]))
  AM_CONDITIONAL([GL_COND_LIBTOOL], [true])
  gl_cond_libtool=true
  gl_source_base='lib'
changequote(,)dnl
LTALLOCA=`echo "$ALLOCA" | sed 's/\.[^.]* /.lo /g;s/\.[^.]*$/.lo/'`
changequote([, ])dnl
AC_SUBST([LTALLOCA])
  gl_FUNC_ALLOCA
  gl_ASSERT
  gl_FUNC_ATEXIT
  AC_FUNC_CALLOC
  gl_CLOSE_STREAM
  gl_MODULE_INDICATOR([close-stream])
  gl_CLOSEOUT
  gl_CONFIG_H
  gl_DIRNAME
  gl_DOUBLE_SLASH_ROOT
  gl_ERROR
  gl_EXITFAIL
  gl_FUNC_FPENDING
  gl_FUNC_FREE
  gl_GETOPT
  dnl you must add AM_GNU_GETTEXT([external]) or similar to configure.ac.
  AM_GNU_GETTEXT_VERSION([0.16.1])
  AC_SUBST([LIBINTL])
  AC_SUBST([LTLIBINTL])
  gl_INLINE
  gl_INTTYPES_H
  gl_IGNORE_UNUSED_LIBRARIES
  gl_LOCALCHARSET
  gl_LONG_OPTIONS
  AC_FUNC_MALLOC
  gl_FUNC_MEMCHR
  gl_FUNC_MEMCMP
  gl_FUNC_MEMCPY
  gl_FUNC_MEMMOVE
  gl_FUNC_MEMSET
  gl_QUOTEARG
  AC_FUNC_REALLOC
  gl_REGEX
  gl_FUNC_RENAME
  gl_FUNC_RPMATCH
  gl_SAFE_READ
  gl_SAFE_WRITE
  gt_TYPE_SSIZE_T
  gl_STDARG_H
  AM_STDBOOL_H
  gl_STDINT_H
  gl_STDLIB_H
  gl_FUNC_STRCSPN
  gl_HEADER_STRING_H
  gl_FUNC_STRNDUP
  gl_STRING_MODULE_INDICATOR([strndup])
  gl_FUNC_STRNLEN
  gl_STRING_MODULE_INDICATOR([strnlen])
  gl_FUNC_STRTOD
  gl_FUNC_STRTOL
  gl_UNISTD_H
  gl_FUNC_UTIME
  gl_WCHAR_H
  gl_WCTYPE_H
  gl_XALLOC
  gl_XSTRNDUP
  m4_popdef([AC_LIBSOURCES])
  m4_popdef([AC_REPLACE_FUNCS])
  m4_popdef([AC_LIBOBJ])
  AC_CONFIG_COMMANDS_PRE([
    gl_libobjs=
    gl_ltlibobjs=
    if test -n "$gl_LIBOBJS"; then
      # Remove the extension.
      sed_drop_objext='s/\.o$//;s/\.obj$//'
      for i in `for i in $gl_LIBOBJS; do echo "$i"; done | sed "$sed_drop_objext" | sort | uniq`; do
        gl_libobjs="$gl_libobjs $i.$ac_objext"
        gl_ltlibobjs="$gl_ltlibobjs $i.lo"
      done
    fi
    AC_SUBST([gl_LIBOBJS], [$gl_libobjs])
    AC_SUBST([gl_LTLIBOBJS], [$gl_ltlibobjs])
  ])
])

# Like AC_LIBOBJ, except that the module name goes
# into gl_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gl_LIBOBJ],
  [gl_LIBOBJS="$gl_LIBOBJS $1.$ac_objext"])

# Like AC_REPLACE_FUNCS, except that the module name goes
# into gl_LIBOBJS instead of into LIBOBJS.
AC_DEFUN([gl_REPLACE_FUNCS],
  [AC_CHECK_FUNCS([$1], , [gl_LIBOBJ($ac_func)])])

# Like AC_LIBSOURCES, except that it does nothing.
# We rely on EXTRA_lib..._SOURCES instead.
AC_DEFUN([gl_LIBSOURCES],
  [])

# This macro records the list of files which have been installed by
# gnulib-tool and may be removed by future gnulib-tool invocations.
AC_DEFUN([gl_FILE_LIST], [
  build-aux/announce-gen
  build-aux/config.rpath
  build-aux/gnupload
  build-aux/link-warning.h
  doc/fdl.texi
  lib/__fpending.c
  lib/__fpending.h
  lib/alloca.c
  lib/alloca_.h
  lib/atexit.c
  lib/basename.c
  lib/calloc.c
  lib/close-stream.c
  lib/close-stream.h
  lib/closeout.c
  lib/closeout.h
  lib/config.charset
  lib/dirname.c
  lib/dirname.h
  lib/error.c
  lib/error.h
  lib/exitfail.c
  lib/exitfail.h
  lib/free.c
  lib/full-write.c
  lib/full-write.h
  lib/getopt.c
  lib/getopt1.c
  lib/getopt_.h
  lib/getopt_int.h
  lib/gettext.h
  lib/inttypes_.h
  lib/localcharset.c
  lib/localcharset.h
  lib/long-options.c
  lib/long-options.h
  lib/malloc.c
  lib/memchr.c
  lib/memcmp.c
  lib/memcpy.c
  lib/memmove.c
  lib/memset.c
  lib/quotearg.c
  lib/quotearg.h
  lib/realloc.c
  lib/ref-add.sin
  lib/ref-del.sin
  lib/regcomp.c
  lib/regex.c
  lib/regex.h
  lib/regex_internal.c
  lib/regex_internal.h
  lib/regexec.c
  lib/rename.c
  lib/rpmatch.c
  lib/safe-read.c
  lib/safe-read.h
  lib/safe-write.c
  lib/safe-write.h
  lib/stdbool_.h
  lib/stdint_.h
  lib/stdlib_.h
  lib/strcspn.c
  lib/string_.h
  lib/stripslash.c
  lib/strndup.c
  lib/strnlen.c
  lib/strtod.c
  lib/strtol.c
  lib/unistd_.h
  lib/utime.c
  lib/version-etc-fsf.c
  lib/version-etc.c
  lib/version-etc.h
  lib/wchar_.h
  lib/wctype_.h
  lib/xalloc-die.c
  lib/xalloc.h
  lib/xmalloc.c
  lib/xstrndup.c
  lib/xstrndup.h
  m4/absolute-header.m4
  m4/alloca.m4
  m4/assert.m4
  m4/atexit.m4
  m4/calloc.m4
  m4/close-stream.m4
  m4/closeout.m4
  m4/codeset.m4
  m4/config-h.m4
  m4/dirname.m4
  m4/dos.m4
  m4/double-slash-root.m4
  m4/error.m4
  m4/exitfail.m4
  m4/extensions.m4
  m4/fpending.m4
  m4/free.m4
  m4/getopt.m4
  m4/gettext.m4
  m4/glibc2.m4
  m4/glibc21.m4
  m4/gnulib-common.m4
  m4/iconv.m4
  m4/include_next.m4
  m4/inline.m4
  m4/intdiv0.m4
  m4/intl.m4
  m4/intldir.m4
  m4/intlmacosx.m4
  m4/intmax.m4
  m4/inttypes-pri.m4
  m4/inttypes.m4
  m4/inttypes_h.m4
  m4/lcmessage.m4
  m4/lib-ignore.m4
  m4/lib-ld.m4
  m4/lib-link.m4
  m4/lib-prefix.m4
  m4/localcharset.m4
  m4/lock.m4
  m4/long-options.m4
  m4/longlong.m4
  m4/mbrtowc.m4
  m4/mbstate_t.m4
  m4/memchr.m4
  m4/memcmp.m4
  m4/memcpy.m4
  m4/memmove.m4
  m4/memset.m4
  m4/nls.m4
  m4/po.m4
  m4/printf-posix.m4
  m4/progtest.m4
  m4/quotearg.m4
  m4/regex.m4
  m4/rename.m4
  m4/rpmatch.m4
  m4/safe-read.m4
  m4/safe-write.m4
  m4/size_max.m4
  m4/ssize_t.m4
  m4/stdarg.m4
  m4/stdbool.m4
  m4/stdint.m4
  m4/stdint_h.m4
  m4/stdlib_h.m4
  m4/strcspn.m4
  m4/string_h.m4
  m4/strndup.m4
  m4/strnlen.m4
  m4/strtod.m4
  m4/strtol.m4
  m4/uintmax_t.m4
  m4/ulonglong.m4
  m4/unistd_h.m4
  m4/utimbuf.m4
  m4/utime.m4
  m4/utimes-null.m4
  m4/utimes.m4
  m4/visibility.m4
  m4/wchar.m4
  m4/wchar_t.m4
  m4/wctype.m4
  m4/wint_t.m4
  m4/xalloc.m4
  m4/xsize.m4
  m4/xstrndup.m4
])
