SOFA_DIR=../../..
TEMPLATE = app
TARGET = sofaCUDA

include($${SOFA_DIR}/sofa.cfg)

DESTDIR = $$SOFA_DIR/bin
CONFIG += $$CONFIGPROJECTGUI
LIBS += $$SOFA_GUI_LIBS
LIBS += $$SOFA_LIBS

SOURCES = Main.cpp
HEADERS = 

RC_FILE = sofa.rc
