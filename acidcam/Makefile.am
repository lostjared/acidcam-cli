bin_PROGRAMS=acidcam
acidcam_SOURCES=acidcam-cli.cpp acidcam-cli.hpp acidcam-cli-main.cpp
AM_CPPFLAGS= -O2 -std=c++11  $(all_includes) $(CV_INCLUDE) $(AC_CFLAGS)
acidcam_LDADD= $(AC_LIBS) $(CV_LIBS) -ldl
