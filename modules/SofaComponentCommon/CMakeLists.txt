cmake_minimum_required(VERSION 2.8.12)
project(SofaComponentCommon)

set(HEADER_FILES
    config.h
    initComponentCommon.h
)

set(SOURCE_FILES
    initComponentCommon.cpp
)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC SofaEigen2Solver SofaDeformable SofaExplicitOdeSolver SofaImplicitOdeSolver SofaLoader SofaMeshCollision SofaObjectInteraction SofaRigid SofaSimpleFem)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-DSOFA_BUILD_COMPONENT_COMMON")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

sofa_install_targets(SofaCommon ${PROJECT_NAME} ${PROJECT_NAME})
