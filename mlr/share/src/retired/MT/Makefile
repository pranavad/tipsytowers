BASE   = ../..
NAME   = $(shell basename `pwd`)
OUTPUT = lib$(NAME).so

GTK = 1
GTKGL = 1
QHULL = 1
GRAPHVIZ = 1
DEPEND = Core Ors Gui Optim extern_ply

SRCS = $(shell find . -maxdepth 1 -name '*.cpp' )
OBJS = $(SRCS:%.cpp=%.o)

#OPTIM=fast

include $(BASE)/build/generic.mk
