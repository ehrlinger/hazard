# Copyright (C) 2000 The Cleveland Clinic Foundation <hazard@bio.ri.ccf.org>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
AM_CPPFLAGS = $(EXTRA_INCLUDES) -I$(top_srcdir)/src/common

noinst_LIBRARIES = libhazpred.a

noinst_HEADERS = hzpe.h  hzpi.h  hzpo.h  hzpp.h hzp_calc_parm_drv.h \
	hzp_calc_intcp_drv.h hzd_calc_norinv.h hzp_calc_XsX.h \
	hzp_calc_log_fn.h hzp_init_CL.h hzp_init_preds.h hzp_pred_time_0.h \
	hzp_calc_scale.h hzp_calc_fn.h hzp_calc_survival.h hzp_calc_hazard.h \
	hzp_calc_srv_CL.h hzp_calc_haz_CL.h hzp_refmt_cov.h \
	hzp_calc_intcp_subr.h hzp_time_parm_drv.h hzpm.h hazpred.h


libhazpred_a_SOURCES = hzpe.c  hzpi.c  hzpo.c  hzpp.c hzp_calc_parm_drv.c \
	hzp_calc_intcp_drv.c hzd_calc_norinv.c hzp_calc_XsX.c \
	hzp_calc_log_fn.c hzp_init_CL.c hzp_init_preds.c hzp_pred_time_0.c \
	hzp_calc_scale.c hzp_calc_fn.c hzp_calc_survival.c hzp_calc_hazard.c \
	hzp_calc_srv_CL.c hzp_calc_haz_CL.c hzp_refmt_cov.c \
	hzp_calc_intcp_subr.c hzp_time_parm_drv.c hzpm.c
