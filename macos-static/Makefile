
#generic Makefile for macOS

OBJ=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
#OPECV_LIBS= ./static/*a
CFLAGS= -F. -mmacosx-version-min=10.13 -std=c++11 -O2 -I./macos/include -I.
LDFLAGS=-mmacosx-version-min=10.13 -framework VideoDecodeAcceleration  -lblas -lcblas -lclapack -llapack -lf77lapack -framework CoreVideo -framework CoreMedia -framework OpenGL -framework Cocoa -framework IOKit -framework AVKit -framework AVFoundation  -framework OpenCL  -lbz2  macos/cvlibs/*a -framework Cocoa -framework AppKit -framework Foundation

%.o: %.cpp
	$(CXX) $(CFLAGS) $< -c -o $@

acidcam-cli: $(OBJ) *.hpp
	make -C libacidcam
	$(CXX) -std=c++11 -o acidcam-cli $(OBJ)  libacidcam/libacidcam.a $(LDFLAGS) -F. 

all: acidcam-cli

.PHONY: clean
clean:
	rm -f acidcam-cli *.o

