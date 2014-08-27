# Copyright (C) 2000 The Cleveland Clinic Foundation <hazard@bio.ri.ccf.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES = $(EXTRA_INCLUDES) \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/utils \
	-I$(top_srcdir)/src/stat \
	-I$(top_srcdir)/src/llike \
	-I$(top_srcdir)/src/optim \
	-I$(top_srcdir)/src/model

noinst_LIBRARIES = libvcov.a

noinst_HEADERS = chlskm.h choslv.h dcovar.h setdll.h

libvcov_a_SOURCES = chlskm.c choslv.c dcovar.c setdll.c
