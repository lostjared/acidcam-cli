AC_PREREQ([2.64])
AC_INIT(acidcam, 1.0, lostjared@lostsidedead.com)
AC_CONFIG_HEADERS(config.h)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE
AM_PROG_AS
AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

CV_INCLUDES=`pkg-config opencv --cflags`
CV_LIBS=`pkg-config opencv --libs`

echo $CV_INCLUDES
echo $CV_LIBS

AC_SUBST(CV_INCLUDES)
AC_SUBST(CV_LIBS)

AC_OUTPUT(Makefile acidcam/Makefile)
