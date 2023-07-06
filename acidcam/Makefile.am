bin_PROGRAMS=acidcam
acidcam_SOURCES=acidcam-cli.cpp acidcam-cli.hpp acidcam-cli-main.cpp ffmpeg_write.cpp
AM_CPPFLAGS= -O2 -std=c++11  $(all_includes) $(CV_INCLUDES) $(AC_CFLAGS) $(METACALL_FLAGS)
acidcam_LDADD= $(AC_LIBS) $(CV_LIBS) $(METACALL_LDFLAGS) -ldl
