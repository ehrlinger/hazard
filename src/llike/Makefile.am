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
	-I$(top_srcdir)/src/vcov

noinst_LIBRARIES = libllike.a

noinst_HEADERS=setcoe.h setlik.h setobj.h constp.h consrv.h \
	setcoe_calc_scaling.h setcoe_setup_indices.h setcoe_obs_loop.h

libllike_a_SOURCES = setcoe.c setlik.c setobj.c constp.c consrv.c \
	setcoe_calc_scaling.c setcoe_setup_indices.c setcoe_obs_loop.c