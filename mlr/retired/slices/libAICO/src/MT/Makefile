BASE   = ../..
NAME   = $(shell basename $(BASE))
OUTPUT = ../$(NAME).so
CXXFLAGS += -fPIC
LIBS   += -lrt

#OPTIM  = fast_debug
OPTIM  = debug

ANN = 1
QHULL = 1
FREEGLUT = 1
SWIFT = 1
LAPACK = 1

CXXFLAGS += -DMLR_IMPLEMENT_TEMPLATES -DMLR_NO_THREADS #causes all *.cpp to implement the used templates

OBJS  = $(shell find . -maxdepth 1 \( -name '*.cpp' -and -not -name '*_t.cpp' -and -not -name '*_instantiate.cpp' \) )
OBJS := $(OBJS:%.cpp=%.o)

include $(BASE)/build/generic.mk
