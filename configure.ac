# -*- Autoconf -*-
#
#

AC_INIT([SST Spatter],[-dev],[sst@sandia.gov])

AC_PREREQ([2.59])
AC_COPYRIGHT([Copyright National Technology and Engineering Solutions of Sandia (NTESS), 2004-2024])

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])
AM_INIT_AUTOMAKE([1.9.6 foreign dist-bzip2 subdir-objects no-define tar-pax])

# If Automake supports silent rules, enable them.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_PROG_AS

AC_PROG_CC
AM_PROG_CC_C_O
m4_if(m4_defn([AC_AUTOCONF_VERSION]), 2.71,, m4_defn([AC_AUTOCONF_VERSION]), 2.70,, [AC_PROG_CC_C99])
AC_C_INLINE
AC_PROG_MAKE_SET

AC_PROG_CXX
AC_CACHE_SAVE

AC_PROG_CPP
AC_CANONICAL_HOST

AC_CACHE_SAVE

LT_CONFIG_LTDL_DIR([src/libltdl])
LT_INIT([shared disable-static dlopen])
LTDL_INIT([recursive])

AC_CACHE_SAVE

SST_CORE_CHECK_INSTALL()

SST_CHECK_SPATTER()

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  tests/Makefile
])

AC_OUTPUT
