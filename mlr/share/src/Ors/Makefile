BASE   = ../..
NAME   = $(shell basename `pwd`)
OUTPUT = lib$(NAME).so

GL = 1
PHYSX = 1
ODE = 0
DEPEND = Core Geo Algo Gui extern_SWIFT
CXXFLAGS += -Wno-unknown-pragmas

SRCS = $(shell find . -maxdepth 1 -name '*.cpp' )
OBJS = $(SRCS:%.cpp=%.o)

include $(BASE)/build/generic.mk
