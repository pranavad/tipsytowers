BASE   = ../../..
NAME   = $(shell basename `pwd`)
OUTPUT = libextern_$(NAME).a

CXXFLAGS += -O3 -fPIC -Wno-deprecated

SRCS = $(shell find . -maxdepth 1 -name '*.cpp' )
OBJS = $(SRCS:%.cpp=%.o)

include $(BASE)/build/generic.mk
