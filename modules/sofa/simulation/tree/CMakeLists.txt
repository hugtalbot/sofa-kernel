cmake_minimum_required(VERSION 2.8.12)
project(SofaSimulationTree)

set(HEADER_FILES
    ExportDotVisitor.h
    GNode.h
    GNodeMultiMappingElement.h
    GNodeVisitor.h
    TreeSimulation.h
    init.h
)

set(SOURCE_FILES
    ExportDotVisitor.cpp
    GNode.cpp
    GNodeMultiMappingElement.cpp
    GNodeVisitor.cpp
    TreeSimulation.cpp
    init.cpp
)

if(SOFA-MISC_SMP)
    list(APPEND HEADER_FILES SMPSimulation.h)
    list(APPEND SOURCE_FILES SMPSimulation.cpp)
endif()

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaSimulationCommon)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-DSOFA_BUILD_SIMULATION_TREE")

sofa_install_targets(SofaSimulation ${PROJECT_NAME} ${PROJECT_NAME})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/" DESTINATION "include/sofa/simulation/tree"
        FILES_MATCHING PATTERN "*.h" PATTERN "*.inl")
