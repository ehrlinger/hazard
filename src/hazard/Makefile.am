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
AM_YFLAGS = -d

# In order to run purify or purecov, setenv PURIFY pur????
# Then make hazard.purify
noinst_PROGRAMS = hazard.purify

hazard_purify_SOURCES = $(hazard_exe_SOURCES)
hazard_purify_LINK = $(PURIFY) $(LINK)

# The standard build rules
# included header files from the source location
AM_CPPFLAGS = $(EXTRA_INCLUDES) \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/utils \
	-I$(top_srcdir)/src/vars \
	-I$(top_srcdir)/src/vcov \
	-I$(top_srcdir)/src/model \
	-I$(top_srcdir)/src/llike \
	-I$(top_srcdir)/src/optim \
	-I$(top_srcdir)/src/stat

# Library pointers, libs are constructed in the build locations
AM_LDFLAGS = -L$(top_builddir)/src/vars \
	-L$(top_builddir)/src/vcov \
	-L$(top_builddir)/src/stat \
	-L$(top_builddir)/src/optim \
	-L$(top_builddir)/src/model \
	-L$(top_builddir)/src/llike \
	-L$(top_builddir)/src/utils \
	-L$(top_builddir)/src/common

LDADD = -lvars -lvcov -lstat -loptim -lmodel -lllike -lutils -lcommon -lm

# The production executable
bin_PROGRAMS = hazard.exe

# A list  of header files to include in the distribution
noinst_HEADERS = alocmem.h alocwrk.h cntobs.h evntprc.h finprmx.h getconc.h \
	getrisk.h hazard.h hazard_y.h hazpprc.h hazrd2.h hazrd3.h \
	hazrd4.h hazrerr.h hzrbomb.h hzrg.h hzri.h hzrinc.h \
	icnsprc.h initprz.h lcnsprc.h memclc1.h memclc2.h opnfils.h parmprc.h \
	phasidx.h przconc.h rcnsprc.h readc1.h readc2.h readc3.h readct.h \
	readlc.h readobs.h readt.h readwt.h setconc.h setidxs.h setobs.h \
	setparmno.h setprmf.h set_restrict.h setrisk.h setstat.h shape.h \
	stpwprc.h theta_x.h timeprc.h vfynvar.h wghtprc.h  \
	varsrpt.h concrpt.h results.h obsstat.h vrpt.h vrpt_l.h vrpt_d.h \
	vrpt_dl.h vput.h vput_l.h vput_d.h vput_dl.h varterm.h varnote.h \
	crpt.h crpt_d.h crpt_l.h crpt_dl.h cput.h cput_v.h cput_d.h cput_l.h \
	cput_dl.h cput_vv.h outmods.h outemod.h outcmod.h outlmod.h outflgs.h \
	cmpmeth.h modterm.h ismuchg.h outflag.h outequn.h outparm.h \
	writeOutputDatafile.h stmtprc.h

# The source files required to build the executable.
hazard_exe_SOURCES = alocmem.c hazard.c hazrerr.c memclc1.c readc1.c readwt.c \
	setrisk.c wghtprc.c alocwrk.c hazpprc.c hzrbomb.c memclc2.c readc2.c \
	setconc.c setstat.c cntobs.c  hzrg.h opnfils.c hzri.c \
	readc3.c  setidxs.c stmtprc.c evntprc.c hazrd2.c  icnsprc.c \
	parmprc.c readct.c  setobs.c  stpwprc.c finprmx.c hazrd3.c  initprz.c \
	phasidx.c readlc.c  setparmno.c theta_x.c getconc.c hazrd4.c \
	lcnsprc.c przconc.c readobs.c setprmf.c timeprc.c getrisk.c  \
	rcnsprc.c readt.c   set_restrict.c vfynvar.c hazard_y.y hazard_l.l \
	hzrg.c varsrpt.c concrpt.c results.c obsstat.c vrpt.c vrpt_l.c \
	vrpt_d.c vrpt_dl.c vput.c vput_l.c vput_d.c vput_dl.c varterm.c \
	varnote.c crpt.c crpt_d.c crpt_l.c crpt_dl.c cput.c cput_v.c cput_d.c \
	cput_l.c cput_dl.c cput_vv.c outmods.c outemod.c outcmod.c outlmod.c \
	outflgs.c cmpmeth.c modterm.c ismuchg.c outflag.c outequn.c outparm.c \
	writeOutputDatafile.c shape.c
