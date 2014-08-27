# Copyright (C) 2000 The Cleveland Clinic Foundation <hazard@bio.ri.ccf.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPO

# Create this CCLD definition (default is just $(CC)) to enable
# the use of rational tools on the code.
INCLUDES = $(EXTRA_INCLUDES) \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/model \
	-I$(top_srcdir)/src/llike

noinst_LIBRARIES = libutils.a

noinst_HEADERS = hzr_calc_loge_mu.h hzr_cum_haz_calc.h hzr_cum_haz_func.h \
	hzr_parm.h hzr_qdiv.h hzr_qextd.h hzr_qinit.h hzr_qmul.h hzr_qneg.h \
	hzr_qsum.h hzr_qtod.h hzr_set_error.h hzr_set_parm_err.h hzr_xmul.h  

libutils_a_SOURCES = hzr_calc_loge_mu.c hzr_cum_haz_calc.c hzr_cum_haz_func.c \
	hzr_parm.c hzr_qdiv.c hzr_qextd.c hzr_qinit.c hzr_qmul.c hzr_qneg.c \
	hzr_qsum.c hzr_qtod.c hzr_set_error.c hzr_set_parm_err.c