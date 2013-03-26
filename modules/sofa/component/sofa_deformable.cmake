cmake_minimum_required(VERSION 2.8)

project("SofaDeformable")

include(${SOFA_CMAKE_DIR}/pre.cmake)

set(HEADER_FILES

    initDeformable.h 
    forcefield/FastTriangularBendingSprings.h 
    forcefield/FastTriangularBendingSprings.inl 
    forcefield/QuadularBendingSprings.h 
    forcefield/QuadularBendingSprings.inl 
    forcefield/RestShapeSpringsForceField.h 
    forcefield/RestShapeSpringsForceField.inl 
    forcefield/TriangularBendingSprings.h 
    forcefield/TriangularBendingSprings.inl 
    forcefield/TriangularBiquadraticSpringsForceField.h 
    forcefield/TriangularBiquadraticSpringsForceField.inl 
    forcefield/TriangularQuadraticSpringsForceField.h 
    forcefield/TriangularQuadraticSpringsForceField.inl 
    forcefield/TriangularTensorMassForceField.h 
    forcefield/TriangularTensorMassForceField.inl 
    interactionforcefield/FrameSpringForceField.h 
    interactionforcefield/FrameSpringForceField.inl 
    interactionforcefield/MeshSpringForceField.h 
    interactionforcefield/MeshSpringForceField.inl 
    interactionforcefield/QuadBendingSprings.h 
    interactionforcefield/QuadBendingSprings.inl 
    interactionforcefield/RegularGridSpringForceField.h 
    interactionforcefield/RegularGridSpringForceField.inl 
    interactionforcefield/SpringForceField.h 
    interactionforcefield/SpringForceField.inl 
    interactionforcefield/StiffSpringForceField.h 
    interactionforcefield/StiffSpringForceField.inl 
    interactionforcefield/TriangleBendingSprings.h 
    interactionforcefield/TriangleBendingSprings.inl 
    interactionforcefield/VectorSpringForceField.h 
    interactionforcefield/VectorSpringForceField.inl

    )
    
set(SOURCE_FILES

    initDeformable.cpp 
    forcefield/FastTriangularBendingSprings.cpp 
    forcefield/QuadularBendingSprings.cpp 
    forcefield/RestShapeSpringsForceField.cpp 
    forcefield/TriangularBendingSprings.cpp 
    forcefield/TriangularBiquadraticSpringsForceField.cpp 
    forcefield/TriangularQuadraticSpringsForceField.cpp 
    forcefield/TriangularTensorMassForceField.cpp 
    interactionforcefield/FrameSpringForceField.cpp 
    interactionforcefield/MeshSpringForceField.cpp 
    interactionforcefield/QuadBendingSprings.cpp 
    interactionforcefield/RegularGridSpringForceField.cpp 
    interactionforcefield/SpringForceField.cpp 
    interactionforcefield/StiffSpringForceField.cpp 
    interactionforcefield/TriangleBendingSprings.cpp 
    interactionforcefield/VectorSpringForceField.cpp

    )
    
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

set(COMPILER_DEFINES "SOFA_BUILD_DEFORMABLE")
set(LINKER_DEPENDENCIES SofaBaseTopology)

include(${SOFA_CMAKE_DIR}/post.cmake)
