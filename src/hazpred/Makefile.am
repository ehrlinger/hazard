# Copyright (C) 2000 The Cleveland Clinic Foundation <hazard@bio.ri.ccf.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law;
# without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPO
# Create this CCLD definition (default is just $(CC)) 
# to enable the use of rational tools on the code.
INCLUDES = $(EXTRA_INCLUDES) -I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/libhazp

AM_LDFLAGS = -L$(top_builddir)/src/common -L$(top_builddir)/src/libhazp
LDADD = -lhazpred -lcommon -lm

AM_YFLAGS = -d

bin_PROGRAMS = hazpred

noinst_HEADERS = addvars.h cntobsh.h defvars.h getflag.h hazpprc.h hazverr.h \
	idproc.h obsloop.h opnfils.h stmtprc.h alocmem.h cntobsi.h fmtverr.h \
	gethazr.h hazpred_y.h initprz.h obsverr.h prederr.h timeprc.h

hazpred_SOURCES = addvars.c defvars.c hazpprc.c hazverr.c idproc.c \
	opnfils.c alocmem.c fmtverr.c hazpred.c initprz.c prederr.c cntobsh.c \
	getflag.c hazpred_l.l obsloop.c stmtprc.c cntobsi.c gethazr.c \
	hazpred_y.y obsverr.c timeprc.c

