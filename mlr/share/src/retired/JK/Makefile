BASE   = ../..
NAME   = JK
OUTPUT = lib$(NAME).so

MAKEMODE = mlrlib

#CXXFLAGS += -DMLR_IMPLEMENT_TEMPLATES

SRCS = $(shell find . -maxdepth 2 \( -name '*.cpp' -and -not -name '*_t.cpp' -and -not -name '*_instantiate.cpp' \) )
OBJS = $(SRCS:%.cpp=%.o)

include $(BASE)/build/generic.mk
