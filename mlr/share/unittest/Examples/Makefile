BASE = ../..

LIBS += -Wl,--no-as-needed -lgtest
_LIBS = $(shell find $(BASE)/examples -mindepth 3 -maxdepth 3 -name 'lib_test.so' )
LIBS += $(_LIBS:%=-l:%)

include $(BASE)/build/generic.mk
