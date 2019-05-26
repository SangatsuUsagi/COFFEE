#
# Common OpenGLES Framework For Embedded boards and Emulated desktops
#
cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
# Check GLES_ROOT has set
if(NOT DEFINED ENV{COFFEE_ROOT})
	message(FATAL_ERROR "Please set COFFEE_ROOT environment variable")
endif()
message("COFFEE_ROOT:" $ENV{COFFEE_ROOT})
# Set build type
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "CMAKE_BUILD_TYPE")
	set(CMAKE_VERBOSE_MAKEFILE 1)
endif()
message(STATUS "Set CMAKE_BUILD_TYPE to ${CMAKE_BUILD_TYPE}")
# Get rid of ZERO_CHECK
set(CMAKE_SUPPRESS_REGENERATION true)
# Force to use out-of-source build
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
# Project settings
project(COFFEE)
set (CMAKE_MODULE_PATH "$ENV{COFFEE_ROOT}/cmake" CACHE STRING "CMAKE_MODULE_PATH")
# Angle libraries
add_subdirectory($ENV{COFFEE_ROOT}/angle/common)
add_subdirectory($ENV{COFFEE_ROOT}/angle/util)
add_subdirectory($ENV{COFFEE_ROOT}/angle/sample_util)
# Support libraries
add_subdirectory($ENV{COFFEE_ROOT}/utils)
# Sample applications
add_subdirectory($ENV{COFFEE_ROOT}/HelloTriangle)
add_subdirectory($ENV{COFFEE_ROOT}/ParticleSystem)
add_subdirectory($ENV{COFFEE_ROOT}/GLES2jni)
add_subdirectory($ENV{COFFEE_ROOT}/GLES2jniTex)
add_subdirectory($ENV{COFFEE_ROOT}/PointSprite)
add_subdirectory($ENV{COFFEE_ROOT}/ModelViewer)
