BASE = $(shell pwd)/../../..

DEPEND = Core MDP

ifeq ($(QT),1)
default: mdp.exe
OBJS := gui_moc.o main.o
gui_moc.o: gui_ui.h gui_moc.cpp
endif

CLEAN = *_moc.cpp *_ui.h MT.log

include $(BASE)/build/generic.mk

