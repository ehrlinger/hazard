# Copyright (C) 2000 The Cleveland Clinic Foundation <hazard@bio.ri.ccf.org>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_PREREQ(2.57)
# AC_INIT must be the first thing done.
# AC_INIT(Hazard_Package, 4.3.0)
AC_INIT([Hazard_Package], [4.3.1])
AC_CONFIG_SRCDIR([./])
AM_INIT_AUTOMAKE

# Set version numbers, these are used to define the VERSION
# macro in the config.h file. We use this to increment versioning.
MAJ_VER=4
MIN_VER=3
MIC_VER=0

# Declare the directory variables,
# these are used to install the hazard package documentation
# (docsdir)
#
# Each variable should match the name of the directory we are
# installing into.
maindir='${prefix}'
docsdir='${prefix}/docs'
docs_userguidesdir='${docsdir}/user_guides'
docs_hazardfilesdir='${docs_userguidesdir}/hazard_files'
docs_overviewfilesdir='${docs_userguidesdir}/overview_files'
docs_hazpredfilesdir='${docs_userguidesdir}/hazpred_files'

# This is for the hazard package support macros
macrosdir='${prefix}/macros'

# These are for the hazard sas example s
examplesdir='${prefix}/examples'
examples_datadir='${examplesdir}/data'
examples_sasestdir='${examplesdir}/sasest'
examples_ihddir='${examples_sasestdir}/ihd'
examples_outputdir='${examplesdir}/output'

# Define them...
# If we don't define the dir, the variable will be invalid in the
# Makefile.am file.
AC_SUBST(maindir)
AC_SUBST(docsdir)
#AC_SUBST(imagesdir)dnl
AC_SUBST(docs_userguidesdir)
AC_SUBST(docs_hazardfilesdir)
AC_SUBST(docs_overviewdir)
AC_SUBST(docs_hazpredfilesdir)
AC_SUBST(macrosdir)
AC_SUBST(examplesdir)
AC_SUBST(examples_datadir)
AC_SUBST(examples_sasestdir)
AC_SUBST(examples_ihddir)
AC_SUBST(examples_outputdir)

# Dump a message saying what we are doing.
echo "configuring the Hazard $MAJ_VER.$MIN_VER.$MIC_VER system"

# AC_CANONICAL_SYSTEM
AC_SUBST(MAJ_VER)dnl
AC_SUBST(MIN_VER)dnl
AC_SUBST(MIC_VER)dnl

# Use automake
#AM_INIT_AUTOMAKE(hazard,$MAJ_VER.$MIN_VER.$MIC_VER)

# Save version and package information in the config.h file
AM_CONFIG_HEADER(config.h)

# Look for the C compiler.
AC_PROG_CC
AC_PROG_CPP
AC_AIX
AC_ISC_POSIX
AC_MINIX
AC_HEADER_STDC

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h malloc.h memory.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor memset pow sqrt strchr strspn])

#AC_PROG_LIBTOOL

# We don't need C++ for the hazard package... yet.
#
# This macro calls
#	AC_PROG_CXX
# 	AC_PROG_CXXCPP

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcommon':
#AC_CHECK_LIB([common], [hzd_Error])
# FIXME: Replace `main' with a function in `-lllike':
#AC_CHECK_LIB([llike], [setcoe])
# FIXME: Replace `main' with a function in `-lmodel':
#AC_CHECK_LIB([model], [DTRSMU_mu_to_theta])
# FIXME: Replace `main' with a function in `-loptim':
#AC_CHECK_LIB([optim], [setoptim])
# FIXME: Replace `main' with a function in `-lstat':
#AC_CHECK_LIB([stat], [NORMAL])
# FIXME: Replace `main' with a function in `-lutils':
#AC_CHECK_LIB([utils], [hzr_set_error])
# FIXME: Replace `main' with a function in `-lvars':
#AC_CHECK_LIB([vars], [BACKW])
# FIXME: Replace `main' with a function in `-lvcov':
#AC_CHECK_LIB([vcov], [DAGRAD])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [pow])

# Checks for header files.
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h limits.h malloc.h memory.h stdlib.h string.h strings.h unistd.h math.h stdio.h asm/byteorder.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor memset pow sqrt strchr])

# Look for the C compiler.
AC_PROG_CC

# Are we on CYGWIN?
#
# If we are compiling for the PC, we want to have the CYGWIN
# variable defined. PC/SAS opens a execution window and it
# would be nice to dump some execution information to tell the
# user something is happening.
#AC_CYGWIN
AC_CHECK_LIB([automode], [automode_create_write_struct_2])

# Then test:
AS_IF([test "$ac_cv_lib_automode_automode_create_write_struct_2" = yes], [HAVE_AUTOMODE=1], [HAVE_AUTOMODE=0])

# Or set conditional:
AM_CONDITIONAL([USE_LIBPNG], [test "$ac_cv_lib_automode_automode_create_write_struct_2" = yes])

# What install program?
AC_PROG_INSTALL

# We are making libraries.
AC_PROG_RANLIB

# What LEX program?
AM_PROG_LEX

# What YACC program?
AC_PROG_YACC

# To determine endianness.
AC_C_BIGENDIAN

# To make sure we get the correct extension naming
AC_EXEEXT

# Change the default installation directory. Since most users
# are not admins... We may change this later. Right now point it to
# a hazard directory in your HOME dir.
# AC_PREFIX_DEFAULT($HOME/hazard)

# What are we making?
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/blackbox/Makefile
                 src/common/Makefile
                 src/hazard/Makefile
                 src/hazpred/Makefile
                 src/llike/Makefile
                 src/libhazp/Makefile
                 src/model/Makefile
                 src/optim/Makefile
                 src/scripts/Makefile
                 src/stat/Makefile
                 src/utils/Makefile
                 src/vars/Makefile
                 src/vcov/Makefile
                 macros/Makefile
                 docs/Makefile
                 docs/user_guides/Makefile
                 docs/user_guides/overview_files/Makefile
                 docs/user_guides/hazard_files/Makefile
                 docs/user_guides/hazpred_files/Makefile
                 examples/Makefile
                 examples/data/Makefile
                 examples/sasest/Makefile
                 examples/sasest/ihd/Makefile
                 examples/output/Makefile
                 ])

AC_OUTPUT
