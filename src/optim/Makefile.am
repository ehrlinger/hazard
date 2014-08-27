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
	-I$(top_srcdir)/src/vcov \
	-I$(top_srcdir)/src/llike

noinst_LIBRARIES = liboptim.a
noinst_HEADERS = bfgsfa.h dnmr2.h fdgrad.h ihesfa.h linesr.h rquad.h uminck.h \
	umstp0.h cndest.h dqdot.h fdhess.h jacrot.h qrupdt.h setoptim.h \
	umstop.h

liboptim_a_SOURCES = bfgsfa.c dnmr2.c  fdgrad.c ihesfa.c linesr.c rquad.c \
	uminck.c umstp0.c cndest.c dqdot.c  fdhess.c jacrot.c qrupdt.c \
	setoptim.c umstop.c
