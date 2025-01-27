# -*- Makefile -*-
#
#

AM_CPPFLAGS += \
	$(SPATTER_CPPFLAGS) \
	-I$(top_srcdir)/src

compdir = $(pkglibdir)
comp_LTLIBRARIES = libsstSpatter.la

libsstSpatter_la_SOURCES = \
	mirandaEvent.h \
	mirandaGenerator.h \
	mirandaCPU.cc \
	mirandaCPU.h \
	mirandaMemMgr.h \
	generators/spatterGenerator.h \
	generators/spatterGenerator.cc

libsstSpatter_la_LDFLAGS = -module -avoid-version \
	$(SPATTER_LDFLAGS) $(SPATTER_LIB) -Wl,-R$(SPATTER_LIBDIR)

install-exec-hook:
	$(SST_REGISTER_TOOL) sstSpatter  			sstSpatter_LIBDIR=$(abs_srcdir)/.libs
	$(SST_REGISTER_TOOL) SST_ELEMENT_SOURCE     sstSpatter=$(abs_srcdir)
	$(SST_REGISTER_TOOL) SST_ELEMENT_TESTS      sstSpatter=$(abs_srcdir)/../tests
