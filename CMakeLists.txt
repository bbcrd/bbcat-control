# Arbitrarily set minimum version.
# This specific version is used to avoid complaints about the 
# cygwin platform not defining WIN32 anymore.
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.12.2)

# The name of our top-level project.
PROJECT( bbcat-control C CXX )

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_RELEASE 2)
set(PROJECT_VERSION_BUILD 1)

#include all the parts that are consistent across all libraries
include(CMakeLists-bbcat.txt)

find_package(bbcat-dsp REQUIRED)
include_directories(${BBCAT_DSP_INCLUDE_DIRS})

# set include directories for storing in .cmake file
set(GLOBAL_INCLUDE_DIRS
	${BBCAT_DSP_INCLUDE_DIRS}
	${INSTALL_INCLUDE_DIR})

# set library directories for storing in .cmake file
set(GLOBAL_LIB_DIRS
	${BBCAT_DSP_LIB_DIRS}
	${INSTALL_LIB_DIR})

# set library directories for linking
link_directories(${GLOBAL_LIB_DIRS})

set(GLOBAL_FLAGS
	${BBCAT_DSP_FLAGS})

ADD_DEFINITIONS(${GLOBAL_FLAGS})

################################################################################
# sources are contained in the src/ directory
ADD_SUBDIRECTORY( src )

################################################################################
# install files for 'share'
install(DIRECTORY "share/"
		DESTINATION "${INSTALL_SHARE_DIR}"
		PATTERN "share/*")
