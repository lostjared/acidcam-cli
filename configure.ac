AC_PREREQ([2.64])
AC_INIT(acidcam, 1.0, lostjared@lostsidedead.com)
AC_CONFIG_HEADERS(config.h)
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE
AM_PROG_AS
AC_LANG_CPLUSPLUS
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK

PKG_CHECK_MODULES(OPENCV, opencv, found=1, found=0)

if test $found = 0; then
AC_MSG_ERROR(Error could not find OpenCV pkg-config file is it installed?)
else
echo "OpenCV Found!"
fi

CV_INCLUDES=$OPENCV_CFLAGS
CV_LIBS=$OPENCV_LIBS

PKG_CHECK_MODULES(ACIDCAM, acidcam, found=1, found=0)

if test $found = 0; then
AC_MSG_ERROR(Error could not find libacidcam pkg-config file is it installed?)
else
echo "libacidcam Found!"
fi

AC_CFLAGS=$ACIDCAM_CFLAGS
AC_LIBS=$ACIDCAM_LIBS

if test $found = 0; then
AC_MSG_ERROR(Error could not find OpenCV pkg-config file is it installed?)
else
echo "OpenCV Found!"
fi

#CV_INCLUDES=`pkg-config opencv --cflags`
#CV_LIBS=`pkg-config opencv --libs`
#echo "includes: $CV_INCLUDES $AC_CFLAGS"
#echo "libraries:  $CV_LIBS $AC_LIBS"

AC_SUBST(CV_INCLUDES)
AC_SUBST(CV_LIBS)
AC_SUBST(AC_CFLAGS)
AC_SUBST(AC_LIBS)

AC_OUTPUT(Makefile acidcam/Makefile)
