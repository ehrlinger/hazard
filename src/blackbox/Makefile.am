hazpred_source = ..
AM_CPPFLAGS= -I$(hazpred_source)/libhazp -I$(hazpred_source)/common
AM_LDFLAGS = -L$(hazpred_source)/libhazp -L$(hazpred_source)/common
LDADD = -lhazpred -lcommon

hzpexmpl_SOURCES= hzpexmpl.c xportHandler.c ibmieee.c xport_version.c

blackbox_SOURCES = blackbox.c xportHandler.c ibmieee.c xport_version.c

xpt2ascii_SOURCES = xpt2ascii.c ibmieee.c xport_version.c

#testieee_SOURCES = testieee.c ibmieee.c

noinst_HEADERS= xportHandler.h ibmieee.h xport_version.h

bin_PROGRAMS=blackbox hzpexmpl xpt2ascii #testieee
