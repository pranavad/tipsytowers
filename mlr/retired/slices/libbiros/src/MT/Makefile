BASE   = $(shell realpath ../..)
NAME   = $(shell basename $(BASE))
OUTPUT = ../$(NAME).so

CONFIGMODE = mlrlib

OBJS  = $(shell find . -maxdepth 1 \( -name '*.cpp' -and -not -name '*_t.cpp' -and -not -name '*_instantiate.cpp' \) )
OBJS := $(OBJS:%.cpp=%.o)

include $(BASE)/build/generic.mk
