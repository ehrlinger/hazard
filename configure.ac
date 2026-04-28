# Copyright (C) 2000 The Cleveland Clinic Foundation <hazard@bio.ri.ccf.org>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_PREREQ([2.69])
# AC_INIT must be the first thing done.
# AC_INIT([Hazard_Package],[4.3.0])
AC_INIT([Hazard_Package],[4.4.4])
AC_CONFIG_SRCDIR([README.md])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([foreign])

# Derive version components from AC_INIT — edit the version there only.
MAJ_VER=`echo "$PACKAGE_VERSION" | cut -d. -f1`
MIN_VER=`echo "$PACKAGE_VERSION" | cut -d. -f2`
MIC_VER=`echo "$PACKAGE_VERSION" | cut -d. -f3`

# Declare the directory variables,
# these are used to install the hazard package documentation
# (docsdir)
#
# Each variable should match the name of the directory we are
# installing into.
maindir='${prefix}'
docsdir='${prefix}/docs'
docs_userguidesdir='${docsdir}/user_guides'

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
AC_MSG_NOTICE([configuring the Hazard $MAJ_VER.$MIN_VER.$MIC_VER system])

AC_SUBST(MAJ_VER)dnl
AC_SUBST(MIN_VER)dnl
AC_SUBST(MIC_VER)dnl

# Use automake
#AM_INIT_AUTOMAKE(hazard,$MAJ_VER.$MIN_VER.$MIC_VER)

# Save version and package information in the config.h file
AC_CONFIG_HEADERS([config.h])

# Optional developer-oriented build flags.
AC_ARG_ENABLE([warnings],
  [AS_HELP_STRING([--enable-warnings],
    [enable extra compiler warnings (-Wall -Wextra -Wno-unused-parameter)])],
  [enable_warnings=$enableval],
  [enable_warnings=no])

AC_ARG_ENABLE([sanitizers],
  [AS_HELP_STRING([--enable-sanitizers],
    [enable AddressSanitizer and UndefinedBehaviorSanitizer])],
  [enable_sanitizers=$enableval],
  [enable_sanitizers=no])

AS_IF([test "x$enable_warnings" = "xyes"], [
  CFLAGS="$CFLAGS -Wall -Wextra -Wno-unused-parameter"
])

AS_IF([test "x$enable_sanitizers" = "xyes"], [
  CFLAGS="$CFLAGS -fsanitize=address,undefined -fno-omit-frame-pointer"
  LDFLAGS="$LDFLAGS -fsanitize=address,undefined -fno-omit-frame-pointer"
])

# Prefer clang on macOS unless the user selected CC explicitly.
AS_IF([test -z "$CC"], [
  case $host_os in
    *darwin* ) AC_CHECK_PROGS([CC], [clang gcc cc], [cc]);;
  esac
])

# Work around shells that export a broken Homebrew gcc-14 on macOS.
# Allow opting out by setting HAZARD_KEEP_CC=1.
AS_IF([test "x$host_os" != "x"], [
  case $host_os in
    *darwin* )
      AS_IF([test "x$CC" = "x/opt/homebrew/bin/gcc-14" && test "x$HAZARD_KEEP_CC" != "x1"], [
        AC_MSG_WARN([ignoring CC=/opt/homebrew/bin/gcc-14 on macOS and falling back to clang; set HAZARD_KEEP_CC=1 to keep it])
        CC=clang
      ])
      ;;
  esac
])

# Look for the build tools.
AC_PROG_CC
AC_PROG_CPP
AC_USE_SYSTEM_EXTENSIONS
AC_PROG_INSTALL
AC_PROG_RANLIB
AM_PROG_LEX([noyywrap])
AC_PROG_YACC
AC_C_BIGENDIAN
AC_EXEEXT

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([float.h limits.h malloc.h memory.h stdlib.h string.h strings.h unistd.h math.h stdio.h asm/byteorder.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_SEARCH_LIBS([pow], [m])
AC_SEARCH_LIBS([strerror], [cposix])
AC_CHECK_FUNCS([floor memset pow sqrt strchr strspn])

# Are we on CYGWIN?
#
# If we are compiling for the PC, we want to have the CYGWIN
# variable defined. PC/SAS opens a execution window and it
# would be nice to dump some execution information to tell the
# user something is happening.
case $host_os in
  *cygwin* ) CYGWIN=yes;;
  * ) CYGWIN=no;;
esac

AC_CHECK_LIB([automode], [automode_create_write_struct_2])

# Then test:
AS_IF([test "$ac_cv_lib_automode_automode_create_write_struct_2" = yes], [HAVE_AUTOMODE=1], [HAVE_AUTOMODE=0])

# Or set conditional:
AM_CONDITIONAL([USE_LIBPNG], [test "$ac_cv_lib_automode_automode_create_write_struct_2" = yes])

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
                 examples/Makefile
                 examples/data/Makefile
                 examples/sasest/Makefile
                 examples/sasest/ihd/Makefile
                 examples/output/Makefile
                 ])

AC_OUTPUT
