load(sofa/pre)

TEMPLATE = lib
TARGET = sofa_base_master_solver

DEFINES += SOFA_BUILD_BASE_MASTER_SOLVER

HEADERS += animationloop/MultiStepAnimationLoop.h \
           animationloop/MultiTagAnimationLoop.h

SOURCES += animationloop/MultiStepAnimationLoop.cpp \
           animationloop/MultiTagAnimationLoop.cpp


# Make sure there are no cross-dependencies
INCLUDEPATH -= $$SOFA_INSTALL_INC_DIR/applications

#exists(component-local.cfg): include(component-local.cfg)

load(sofa/post)
 
