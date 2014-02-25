## This makefile is for gcc-4.1.2
ARNOLDINC := /X/tools/binlinux/arnold/arnold-4.1.3.3/include
ARNOLDLIB := /X/tools/binlinux/arnold/arnold-4.1.3.3/bin

# if your g++ is located somewhere else
GXXLOC    := 
TARGETLOC := /USERS/chichang/tools/cpp/al_shaders/jf-nested-dielectric/build/

INCLUDES := -I$(ARNOLDINC)
LDFLAGS  := -L$(ARNOLDLIB) -lai
CXXFLAGS := -w -O2 -fPIC -DLINUX -D_BOOL -ffast-math -DGCC_VERSION=4.1.2
CXX	     := $(GXXLOC)g++

SOURCES  := jf_nested_dielectric.cpp
OBJECTS  := $(SOURCES:.cpp=.o)
TARGET   := $(TARGETLOC)jf_nested_dielectric.so

$(TARGET) : $(OBJECTS)
	$(CXX) $(inputs) -shared $< -o $@ $(LDFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

all: $(TARGET)

clean:
	rm $(OBJECTS) $(TARGET)


#export ARNOLD_PATH=/X/tools/binlinux/arnold/arnold-4.1.3.3

#g++ -o ydPlasticShader.os -c -fPIC -D_LINUX -I$ARNOLD_PATH/include ydPlasticShader.cpp
#g++ -o ydPlasticShader.so -shared ydPlasticShader.os -L$ARNOLD_PATH/bin -lai

#$ARNOLD_PATH/bin/kick -l /USERS/chichang/tools/cpp/al_shaders/ydPlasticShader.os -info simple

#export ARNOLD_PLUGIN_PATH=/USERS/chichang/tools/cpp/al_shaders
#export MTOA_TEMPLATES_PATH=/USERS/chichang/tools/cpp/al_shaders
