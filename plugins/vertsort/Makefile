CXX_FLAGS=-std=c++11 `pkg-config opencv --cflags`
SOURCE=source.cpp
OUTPUT=vertsort.acf
LDADD=`pkg-config opencv --libs`
all:
	$(CXX) -fPIC $(CXX_FLAGS) -shared $(SOURCE) -o  $(OUTPUT) $(LDADD)
clean:
	rm -f $(OUTPUT)
