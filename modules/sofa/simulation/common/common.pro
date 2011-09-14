# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: ./modules/sofa/simulation/common
# Target is a library:  sofasimulation$$LIBSUFFIX
load(sofa/pre)

TEMPLATE = lib
TARGET = sofasimulation

DEFINES += SOFA_BUILD_SIMULATION_COMMON

INCLUDEPATH += $$ROOT_SRC_DIR/modules

# Make sure there are no cross-dependencies
INCLUDEPATH -= $$ROOT_SRC_DIR/applications

HEADERS += common.h \
           Node.h \
           AnimateBeginEvent.h \
           AnimateEndEvent.h \
           AnimateVisitor.h \   
           BehaviorUpdatePositionVisitor.h \
           CactusStackStorage.h \  
           ClassSystem.h \  
           CleanupVisitor.h \
           CollisionAnimationLoop.h \
           CollisionBeginEvent.h \
           CollisionEndEvent.h \
           CollisionVisitor.h \  
           Colors.h \
           DeactivatedNodeVisitor.h \
           DefaultAnimationLoop.h \
           DefaultVisualManagerLoop.h \
           DeleteVisitor.h \
           ExportGnuplotVisitor.h \
           ExportOBJVisitor.h \
           FindByTypeVisitor.h \
           InitVisitor.h \  
           LocalStorage.h \
#           MasterSolverImpl.h \
           MechanicalComputeEnergyVisitor.h \
           MechanicalMatrixVisitor.h \
	         MechanicalOperations.h \
           MechanicalVPrintVisitor.h \
           MechanicalVisitor.h \
           MutationListener.h \
           ParallelVisitorScheduler.h \
           PauseEvent.h \
           PipelineImpl.h \
           PositionEvent.h\
           PrintVisitor.h \
           PropagateEventVisitor.h \
           ResetVisitor.h \
           Simulation.h \
           SolveVisitor.h \
           StateChangeVisitor.h \
           TopologyChangeVisitor.h \
           TransformationVisitor.h \
           UpdateBoundingBoxVisitor.h \
		   UpdateContextVisitor.h \
           UpdateMappingEndEvent.h \
           UpdateMappingVisitor.h \
		   VectorOperations.h \
           VelocityThresholdVisitor.h \
           Visitor.h \
		   VisitorExecuteFunc.h \
           VisitorScheduler.h \
           VisualVisitor.h \
           WriteStateVisitor.h \
           XMLPrintVisitor.h \
           xml/BaseElement.h \
           xml/ElementNameHelper.h \
           xml/Element.h \
           xml/Element.inl \
           xml/initXml.h \
           xml/NodeElement.h \
           xml/ObjectElement.h \
		   xml/BaseMultiMappingElement.h \
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
           CollisionAnimationLoop.cpp \
           CollisionVisitor.cpp \
           DeactivatedNodeVisitor.cpp \
           DefaultAnimationLoop.cpp \
           DefaultVisualManagerLoop.cpp \
           DeleteVisitor.cpp \
           ExportGnuplotVisitor.cpp \
           ExportOBJVisitor.cpp \
           InitVisitor.cpp \   
#           MasterSolverImpl.cpp \
           MechanicalComputeEnergyVisitor.cpp \
		   MechanicalOperations.cpp \
	       MechanicalMatrixVisitor.cpp \
           MechanicalVPrintVisitor.cpp \
           MechanicalVisitor.cpp \   
           MutationListener.cpp \ 
           ParallelVisitorScheduler.cpp \
           PauseEvent.cpp \
           PipelineImpl.cpp \
           PositionEvent.cpp\
           PrintVisitor.cpp \
           PropagateEventVisitor.cpp \
           ResetVisitor.cpp \
           Simulation.cpp \  
           SolveVisitor.cpp \
           TopologyChangeVisitor.cpp \
           StateChangeVisitor.cpp \   
           TransformationVisitor.cpp \
           UpdateBoundingBoxVisitor.cpp \
		   UpdateContextVisitor.cpp \
           UpdateMappingEndEvent.cpp \
           UpdateMappingVisitor.cpp \
		   VectorOperations.cpp \
           VelocityThresholdVisitor.cpp \
           Visitor.cpp \
           VisitorScheduler.cpp \
           VisualVisitor.cpp \
           WriteStateVisitor.cpp \
           XMLPrintVisitor.cpp \
           xml/BaseElement.cpp \
           xml/ElementNameHelper.cpp \
           xml/initXml.cpp \
           xml/NodeElement.cpp \
           xml/ObjectElement.cpp \
		   xml/BaseMultiMappingElement.cpp \
           xml/AttributeElement.cpp \
           xml/DataElement.cpp \
           xml/XML.cpp 

contains(DEFINES,SOFA_SMP){
HEADERS +=  \
           ChangeListener.h \
           ParallelMechanicalVisitor.h 

SOURCES +=  \
           ChangeListener.cpp \
           ParallelMechanicalVisitor.cpp 
}

load(sofa/post)
