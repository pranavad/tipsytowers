# Symbol grounding Makefile
# Johannes, 09 Dec 2011
#

BASE = ../../share

buildAndLink_RELATIONAL = 1
buildAndLink_JOHANNES = 1
buildAndLink_MTLIB = 1

LIBS += -lbiros -ljk

OBJS  = $(shell find . -maxdepth 1 \( -name '*.cpp' -and -not -name '*_t.cpp' -and -not -name '*_instantiate.cpp' \) )
OBJS := $(OBJS:%.cpp=%.o)

include $(BASE)/build/generic.mk
