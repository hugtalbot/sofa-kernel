# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./modules/sofa/simulation/common
# Target is a library:  sofasimulation$$LIBSUFFIX

HEADERS += common.h \
           Node.h \
           AnimateBeginEvent.h \
           AnimateEndEvent.h \
           AnimateVisitor.h \   
           BehaviorUpdatePositionVisitor.h \
           CactusStackStorage.h \  
           ClassSystem.h \  
           CleanupVisitor.h \
           CollisionVisitor.h \  
           Colors.h \
           DeleteVisitor.h \
           DesactivatedNodeVisitor.h \
           ExportGnuplotVisitor.h \
           ExportOBJVisitor.h \
           FindByTypeVisitor.h \
           InitVisitor.h \  
           InstrumentVisitor.h \
           LocalStorage.h \
           MasterSolverImpl.h \
           MechanicalComputeEnergyVisitor.h \
	   MechanicalMatrixVisitor.h \
           MechanicalVPrintVisitor.h \
           MechanicalVisitor.h \  
           MutationListener.h \
           ParallelVisitorScheduler.h \
           PipelineImpl.h \
           PrintVisitor.h \
           PropagateEventVisitor.h \
           ResetVisitor.h \
           Simulation.h \
           SolveVisitor.h \
           SolverImpl.h \
           StateChangeVisitor.h \
           TopologyChangeVisitor.h \
           TransformationVisitor.h \
           UpdateContextVisitor.h \
           UpdateMappingEndEvent.h \
           UpdateMappingVisitor.h \
           VelocityThresholdVisitor.h \
           Visitor.h \
           VisitorScheduler.h \
           VisualVisitor.h \
           WriteStateVisitor.h \
           XMLPrintVisitor.h \
           xml/BaseElement.h \
           xml/Element.h \
           xml/Element.inl \
           xml/initXml.h \
           xml/NodeElement.h \
           xml/ObjectElement.h \
           xml/AttributeElement.h \
           xml/DataElement.h \
           xml/XML.h 

SOURCES += Node.cpp \
           AnimateBeginEvent.cpp \
           AnimateEndEvent.cpp \
           AnimateVisitor.cpp \
           BehaviorUpdatePositionVisitor.cpp \
           CactusStackStorage.cpp \
           CleanupVisitor.cpp \
           CollisionVisitor.cpp \
           DeleteVisitor.cpp \
           DesactivatedNodeVisitor.cpp \
           ExportGnuplotVisitor.cpp \
           ExportOBJVisitor.cpp \
           InitVisitor.cpp \   
           InstrumentVisitor.cpp \
           MasterSolverImpl.cpp \
           MechanicalComputeEnergyVisitor.cpp \
	   MechanicalMatrixVisitor.cpp \
           MechanicalVPrintVisitor.cpp \
           MechanicalVisitor.cpp \   
           MutationListener.cpp \ 
           ParallelVisitorScheduler.cpp \
           PipelineImpl.cpp \
           PrintVisitor.cpp \
           PropagateEventVisitor.cpp \
           ResetVisitor.cpp \
           Simulation.cpp \  
           SolveVisitor.cpp \
           SolverImpl.cpp \
           TopologyChangeVisitor.cpp \
           StateChangeVisitor.cpp \   
           TransformationVisitor.cpp \
           UpdateContextVisitor.cpp \
           UpdateMappingEndEvent.cpp \
           UpdateMappingVisitor.cpp \
           VelocityThresholdVisitor.cpp \
           Visitor.cpp \
           VisitorScheduler.cpp \
           VisualVisitor.cpp \
           WriteStateVisitor.cpp \
           XMLPrintVisitor.cpp \
           xml/BaseElement.cpp \
           xml/initXml.cpp \
           xml/NodeElement.cpp \
           xml/ObjectElement.cpp \
           xml/AttributeElement.cpp \
           xml/DataElement.cpp \
           xml/XML.cpp 

SOFA_DIR =../../../..
TEMPLATE = lib
include($${SOFA_DIR}/sofa.cfg)
TARGET = sofasimulation$$LIBSUFFIX
CONFIG += $$CONFIGLIBRARIES

CONFIG -= staticlib
CONFIG += dll

DEFINES += SOFA_BUILD_SIMULATION_COMMON

LIBS += $$SOFA_FRAMEWORK_LIBS
LIBS += $$SOFA_EXT_LIBS

# Make sure there are no cross-dependencies
INCLUDEPATH -= $$SOFA_DIR/applications
