"""
A simple and convenient python interface for the ActivityMachine.

::

    ipython -i minimal.py

"""
from __future__ import print_function
import time
import numpy as np
import swig





interface = swig.ActionSwigInterface(False)

# new convenient symbols
for symb in ["rot", "qItself", "pos", "front", "gazeAt", "gripper", "align", "wheels", "vecDiff", "vec"]:
    interface.createNewSymbol(symb)

# don't abort the swig interface on Ctr-C
# the signal must be geristered after the swig interface was initialized
# signal.signal(signal.SIGINT, signal_handler)


###############################################################################
# Helper: access ors structures
def shapes(name=None):
    return interface.getShapeByName(name) if name else interface.getShapeList()


def joints(name=None):
    return interface.getJointByName(name) if name else interface.getJointList()

def bodies(name=None):
    return interface.getBodyByName(name) if name else interface.getBodyList()

def symbols():
    return interface.getSymbols()

def facts():
    return interface.getFacts()


