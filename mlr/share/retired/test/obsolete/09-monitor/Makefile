BASE = $(shell pwd)/../../../share

OBJS = gui_moc.o main.o
#OBJS = main.o

default: x.exe

gui_moc.o: gui_moc.cpp gui_ui.h

FREEGLUT = 1
LAPACK = 1
QT = 1
SCHUNK = 1

CLEAN = *_moc.cpp *_ui.h MT.log

include $(BASE)/build/generic.mk

