BASE   = ../..
NAME   = libnp
OUTPUT = ../$(NAME).so
CXXFLAGS += -fPIC

#OPTIM  = fast_debug
OPTIM  = debug

ANN = 1
QHULL = 1
FREEGLUT = 1
LAPACK = 1
CUDA = 1
OPENCV = 1
BUMBLE = 1

CXXFLAGS += -DNP_IMPLEMENT_TEMPLATES -DMLR_IMPLEMENT_TEMPLATES -DNP_DC1394 

ifndef OBJS
OBJS  = $(shell find . \( -name '*.cpp' -and -not -name '*_t.cpp' -and -not -name '*_instantiate.cpp' \) )
OBJS := $(OBJS:%.cpp=%.o)
endif

include $(BASE)/build/generic.mk
