bin_PROGRAMS=acidcam
INCLUDES= `pkg-config opencv --cflags`
acidcam_SOURCES=acidcam-cli.cpp ac.cpp
AM_CPPFLAGS= -O2 -std=c++11  $(all_includes)
acidcam_LDADD= `pkg-config opencv --libs`
