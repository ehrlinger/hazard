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
AM_CPPFLAGS = $(EXTRA_INCLUDES) -I$(top_srcdir)/src/common

# In order to run purify or purecov, setenv PURIFY pur????
# Then make hazard.purify
noinst_PROGRAMS = hazpred.purify

hazpred_purify_SOURCES = $(hazpred_exe_SOURCES)
hazpred_purify_LINK = $(PURIFY) $(LINK)

AM_LDFLAGS = -L$(top_builddir)/src/common
LDADD = -lcommon -lm

AM_YFLAGS = -d

bin_PROGRAMS = hazpred.exe


noinst_HEADERS = hazpred.h initprz.h opnfils.h hazpprc.h hzp_calc_XsX.h \
	stmtprc.h timeprc.h idproc.h cntobsh.h cntobsi.h alocmem.h gethazr.h \
	defvars.h hazverr.h fmtverr.h prederr.h getflag.h obsverr.h addvars.h \
	obsloop.h hzpi.h hzpm.h hzpe.h hzpp.h hzd_calc_norinv.h hzp_init_CL.h \
	hzp_refmt_cov.h hzpo.h hzp_init_preds.h hzp_pred_time_0.h hazpred_y.h \
	hzp_calc_scale.h hzp_calc_log_fn.h hzp_calc_fn.h hzp_calc_survival.h \
	hzp_calc_hazard.h hzp_calc_parm_drv.h hzp_calc_intcp_drv.h \
	hzp_calc_srv_CL.h hzp_calc_haz_CL.h hzp_time_parm_drv.h \
	hzp_calc_intcp_subr.h


hazpred_exe_SOURCES = hazpred.c hazpred_y.y hazpred_l.l initprz.c opnfils.c \
	hazpprc.c stmtprc.c timeprc.c idproc.c cntobsh.c cntobsi.c \
	alocmem.c gethazr.c defvars.c hazverr.c fmtverr.c prederr.c getflag.c \
	obsverr.c addvars.c obsloop.c hzpi.c hzpm.c hzpe.c hzpp.c hzpo.c  \
	hzd_calc_norinv.c hzp_init_CL.c hzp_refmt_cov.c hzp_init_preds.c \
	hzp_pred_time_0.c hzp_calc_scale.c hzp_calc_log_fn.c hzp_calc_fn.c \
	hzp_calc_survival.c hzp_calc_hazard.c hzp_calc_parm_drv.c \
	hzp_calc_intcp_drv.c hzp_calc_XsX.c hzp_calc_srv_CL.c \
	hzp_calc_haz_CL.c hzp_time_parm_drv.c hzp_calc_intcp_subr.c
