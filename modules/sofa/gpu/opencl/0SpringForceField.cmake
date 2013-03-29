cmake_minimum_required(VERSION 2.8)


list(APPEND HEADER_FILES "OpenCLSpringForceField.h")
list(APPEND HEADER_FILES "OpenCLSpringForceField.inl")

list(APPEND SOURCE_FILES "OpenCLSpringForceField.cpp")

list(APPEND OTHER_FILES "kernels/OpenCLSpringForceField.cl")

