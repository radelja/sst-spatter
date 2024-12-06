dnl -*- Autoconf -*-
dnl vim:ft=config
dnl

AC_DEFUN([SST_sstSpatter_CONFIG], [
	sstSpatter_happy="yes"

  # Use global Stake check
  SST_CHECK_STAKE([],[],[AC_MSG_ERROR([Stake requests but could not be found])])

  SST_CHECK_SPATTER([],[],[AC_MSG_ERROR([Spatter was requested but was not found])])

  AS_IF([test "$sstSpatter_happy" = "yes"], [$1], [$2])
])
