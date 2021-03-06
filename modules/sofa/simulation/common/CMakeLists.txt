cmake_minimum_required(VERSION 2.8.12)
project(SofaSimulationCommon)

set(HEADER_FILES
    AnimateBeginEvent.h
    AnimateEndEvent.h
    AnimateVisitor.h
    BehaviorUpdatePositionVisitor.h
    CactusStackStorage.h
    ClassSystem.h
    CleanupVisitor.h
    CollisionAnimationLoop.h
    CollisionBeginEvent.h
    CollisionEndEvent.h
    CollisionVisitor.h
    Colors.h
    CopyAspectVisitor.h
    DeactivatedNodeVisitor.h
    DefaultAnimationLoop.h
    DefaultVisualManagerLoop.h
    DeleteVisitor.h
    ExportGnuplotVisitor.h
    ExportOBJVisitor.h
    FindByTypeVisitor.h
    InitVisitor.h
    IntegrateBeginEvent.h
    IntegrateEndEvent.h
    LocalStorage.h
    MechanicalComputeEnergyVisitor.h
    MechanicalMatrixVisitor.h
    MechanicalOperations.h
    MechanicalVPrintVisitor.h
    MechanicalVisitor.h
    MutationListener.h
    Node.h
    ParallelVisitorScheduler.h
    PauseEvent.h
    PipelineImpl.h
    PositionEvent.h
    PrintVisitor.h
    PropagateEventVisitor.h
    ReleaseAspectVisitor.h
    ResetVisitor.h
    SceneLoaderFactory.h
    SceneLoaderPHP.h
    SceneLoaderXML.h
    Simulation.h
    SolveVisitor.h
    StateChangeVisitor.h
    TopologyChangeVisitor.h
    TransformationVisitor.h
    UpdateBoundingBoxVisitor.h
    UpdateContextVisitor.h
    UpdateLinksVisitor.h
    UpdateMappingEndEvent.h
    UpdateMappingVisitor.h
    VectorOperations.h
    VelocityThresholdVisitor.h
    Visitor.h
    VisitorExecuteFunc.h
    VisitorScheduler.h
    VisualVisitor.h
    WriteStateVisitor.h
    XMLPrintVisitor.h
    init.h
    xml/AttributeElement.h
    xml/BaseElement.h
    xml/BaseMultiMappingElement.h
    xml/DataElement.h
    xml/Element.h
    xml/Element.inl
    xml/ElementNameHelper.h
    xml/NodeElement.h
    xml/ObjectElement.h
    xml/XML.h
    MechanicalGetMomentumVisitor.h
)

set(SOURCE_FILES
    AnimateBeginEvent.cpp
    AnimateEndEvent.cpp
    AnimateVisitor.cpp
    BehaviorUpdatePositionVisitor.cpp
    CactusStackStorage.cpp
    CleanupVisitor.cpp
    CollisionAnimationLoop.cpp
    CollisionBeginEvent.cpp
    CollisionEndEvent.cpp
    CollisionVisitor.cpp
    CopyAspectVisitor.cpp
    DeactivatedNodeVisitor.cpp
    DefaultAnimationLoop.cpp
    DefaultVisualManagerLoop.cpp
    DeleteVisitor.cpp
    ExportGnuplotVisitor.cpp
    ExportOBJVisitor.cpp
    InitVisitor.cpp
    IntegrateBeginEvent.cpp
    IntegrateEndEvent.cpp
    MechanicalMatrixVisitor.cpp
    MechanicalOperations.cpp
    MechanicalVPrintVisitor.cpp
    MechanicalVisitor.cpp
    MutationListener.cpp
    Node.cpp
    ParallelVisitorScheduler.cpp
    PauseEvent.cpp
    PipelineImpl.cpp
    PositionEvent.cpp
    PrintVisitor.cpp
    PropagateEventVisitor.cpp
    ReleaseAspectVisitor.cpp
    ResetVisitor.cpp
    SceneLoaderFactory.cpp
    SceneLoaderPHP.cpp
    SceneLoaderXML.cpp
    Simulation.cpp
    SolveVisitor.cpp
    StateChangeVisitor.cpp
    TopologyChangeVisitor.cpp
    TransformationVisitor.cpp
    UpdateBoundingBoxVisitor.cpp
    UpdateContextVisitor.cpp
    UpdateLinksVisitor.cpp
    UpdateMappingEndEvent.cpp
    UpdateMappingVisitor.cpp
    VectorOperations.cpp
    VelocityThresholdVisitor.cpp
    Visitor.cpp
    VisitorScheduler.cpp
    VisualVisitor.cpp
    WriteStateVisitor.cpp
    XMLPrintVisitor.cpp
    init.cpp
    xml/AttributeElement.cpp
    xml/BaseElement.cpp
    xml/BaseMultiMappingElement.cpp
    xml/DataElement.cpp
    xml/ElementNameHelper.cpp
    xml/NodeElement.cpp
    xml/ObjectElement.cpp
    xml/XML.cpp
    MechanicalComputeEnergyVisitor.cpp
)

if(SOFA-MISC_SMP)
    list(APPEND HEADER_FILES ChangeListener.h)
    list(APPEND HEADER_FILES ParallelMechanicalVisitor.h)
    list(APPEND SOURCE_FILES ChangeListener.cpp)
    list(APPEND SOURCE_FILES ParallelMechanicalVisitor.cpp)
endif()

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaCore)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-DSOFA_BUILD_SIMULATION_COMMON")

sofa_install_targets(SofaSimulation ${PROJECT_NAME} ${PROJECT_NAME})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/" DESTINATION "include/sofa/simulation/common"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.inl")
