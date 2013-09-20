cmake_policy(SET CMP0015 OLD)

# lib dir
link_directories("${SOFA_LIB_DIR}")
link_directories("${SOFA_LIB_OS_DIR}")
if (NOT SOFA-EXTERNAL_LIBRARY_DIR STREQUAL "")
link_directories("${SOFA-EXTERNAL_LIBRARY_DIR}")
endif()

# include dirs
if(WIN32)
    list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${SOFA_INC_DIR}")
endif()
if(NOT SOFA-EXTERNAL_INCLUDE_DIR STREQUAL "")
    list(APPEND GLOBAL_INCLUDE_DIRECTORIES ${SOFA-EXTERNAL_INCLUDE_DIR})
endif()
list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${SOFA_FRAMEWORK_DIR}")
list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${SOFA_MODULES_DIR}")
list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${SOFA_APPLICATIONS_DIR}")

if(SOFA-MISC_DEVELOPER_MODE)
	list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${SOFA_APPLICATIONS_DEV_DIR}")
endif()

if(SOFA-EXTERNAL_BOOST)
	if(NOT SOFA-EXTERNAL_BOOST_PATH STREQUAL "")
		set(ENV{Boost_DIR} ${SOFA-EXTERNAL_BOOST_PATH})
	endif()
	set(Boost_USE_STATIC_LIBS		OFF)
	set(Boost_USE_MULTITHREADED		ON)
	set(Boost_USE_STATIC_RUNTIME    OFF)
	find_package("Boost" REQUIRED COMPONENTS thread graph system)
	set(Boost_DIR ${SOFA-EXTERNAL_BOOST_PATH} CACHE INTERNAL "Boost root directory" FORCE)
	set(Boost_LIB_DIAGNOSTIC_DEFINITIONS ${Boost_LIB_DIAGNOSTIC_DEFINITIONS} CACHE INTERNAL "Boost lib diagnostic definitions" FORCE)
	set(GLOBAL_COMPILER_DEFINES ${GLOBAL_COMPILER_DEFINES} BOOST_ALL_NO_LIB CACHE INTERNAL "Global Compiler Defines" FORCE)
	list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}")
else()
    list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${SOFA-EXTERNAL_BOOST_PATH}")
endif()


## Zlib (SOFA-EXTERNAL_ZLIB)
if(WIN32 OR XBOX OR PS3)
if(PS3)
	set(ZLIB_LIBRARIES "${SOFA_LIB_OS_DIR}/zlib.a")
	set(ZLIB_INCLUDE_DIR ${SOFA_INC_DIR})
else()
	set(ZLIB_LIBRARIES "zlib")
endif(PS3)
else()
	find_library(ZLIB_LIBRARIES "z")
endif()
set(ZLIB_LIBRARIES ${ZLIB_LIBRARIES} CACHE INTERNAL "ZLib Library")
if (SOFA-EXTERNAL_ZLIB)
  set(ZLIB_LIBRARIES_OPTIONAL ${ZLIB_LIBRARIES} CACHE INTERNAL "ZLib Library")
else()
  set(ZLIB_LIBRARIES_OPTIONAL "" CACHE INTERNAL "ZLib Library (optional)")
endif()
RegisterDependencies(${ZLIB_LIBRARIES} OPTION SOFA-EXTERNAL_ZLIB COMPILE_DEFINITIONS SOFA_HAVE_ZLIB)

# packages and libraries

## opengl / glew / glut
if (NOT SOFA-MISC_NO_OPENGL)
	find_package(OPENGL REQUIRED)
	unset(GLUT_LIBRARIES CACHE) # force remove glut path to be able to switch glut/freeglut
	if(WIN32)
		set(OPENGL_LIBRARIES ${OPENGL_LIBRARIES} "glu32")
		if (SOFA-EXTERNAL_FREEGLUT)
			set(GLUT_LIBRARIES "freeglut")
		else()
			set(GLUT_LIBRARIES "glut32")
		endif()
		if (SOFA-EXTERNAL_GLEW)
			set(GLEW_LIBRARIES "glew32")
		else()
			set(GLEW_LIBRARIES "")
		endif()
	else()
		if(NOT PS3)
		      if (SOFA-EXTERNAL_FREEGLUT)
			      find_package(GLUT REQUIRED)
		      else()
			      if(APPLE)
				    find_library(GLUT_LIBRARIES "GLUT")
			      else()
				      find_library(GLUT_LIBRARIES "glut")
			      endif()
		      endif()
		endif()
		## GLU
		if(UNIX)
			if(NOT APPLE)
				list(APPEND GLUT_LIBRARIES GLU X11)
			endif()
			list(APPEND GLUT_LIBRARIES dl)
		endif()
		list(REMOVE_DUPLICATES GLUT_LIBRARIES)
		if (SOFA-EXTERNAL_GLEW AND NOT PS3)
			find_package(GLEW REQUIRED)
		else()
			set(GLEW_LIBRARIES "")
		endif()
	endif()
else()
	set(OPENGL_LIBRARIES "")
	set(GLUT_LIBRARIES "")
	set(GLEW_LIBRARIES "")
endif()

## libpng
if (SOFA-EXTERNAL_PNG)
	if(WIN32)
		set(PNG_LIBRARIES "libpng")
	else()
		if(SOFA-EXTERNAL_PNG_SPECIFIC_VERSION)
			set(PNG_LIBRARIES "${SOFA-EXTERNAL_PNG_VERSION}")
		else()
			#find_library(PNG_LIBRARIES "png")
			find_package(PNG REQUIRED)
			list(APPEND GLOBAL_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIR}")
		endif()
	endif()
	if(PS3)
		set(PNG_LIBRARIES "${SOFA_LIB_OS_DIR}/libpng.a")
		set(PNG_INCLUDE_DIR ${SOFA_INC_DIR})
	endif(PS3)
else()
	set(PNG_LIBRARIES "")
endif()

set(OPENGL_LIBRARIES ${OPENGL_LIBRARIES} CACHE INTERNAL "OpenGL Library")
set(GLEW_LIBRARIES ${GLEW_LIBRARIES} CACHE INTERNAL "GLEW Library")
set(GLUT_LIBRARIES ${GLUT_LIBRARIES} CACHE INTERNAL "GLUT Library")
set(PNG_LIBRARIES ${PNG_LIBRARIES} CACHE INTERNAL "PNG Library")

RegisterDependencies(${OPENGL_LIBRARIES})
RegisterDependencies(${GLEW_LIBRARIES} OPTION SOFA-EXTERNAL_GLEW COMPILE_DEFINITIONS SOFA_HAVE_GLEW)
if(SOFA-EXTERNAL_FREEGLUT)
	RegisterDependencies(${GLUT_LIBRARIES} COMPILE_DEFINITIONS SOFA_HAVE_FREEGLUT)
else()
	RegisterDependencies(${GLUT_LIBRARIES})
endif()
RegisterDependencies(${PNG_LIBRARIES} OPTION SOFA-EXTERNAL_PNG COMPILE_DEFINITIONS SOFA_HAVE_PNG)

## metis
if(SOFA-EXTERNAL_METIS)
        find_package(METIS REQUIRED)
endif()
RegisterDependencies(${METIS_LIBRARIES} OPTION SOFA-EXTERNAL_METIS COMPILE_DEFINITIONS SOFA_HAVE_METIS)


# enable unit tests
if(SOFA-MISC_TESTS)
	enable_testing()
endif()
