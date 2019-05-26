# Allow the developer to select if Dynamic or Static libraries are built
option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
# Set the LIB_TYPE variable to STATIC
set(LIB_TYPE STATIC)
if(BUILD_SHARED_LIBS)
	# User wants to build Dynamic Libraries, so change the LIB_TYPE variable to CMake keyword 'SHARED'
	set(LIB_TYPE SHARED)
endif(BUILD_SHARED_LIBS)

# Set compiler dependent options
cmake_policy(SET CMP0054 NEW)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	# using Clang
	add_definitions( -DCOMPILER_CLANG -DNDEBUG )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	add_definitions( -DCOMPILER_GCC -DNDEBUG )
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# using Visual Studio C++
	add_definitions( -DNDEBUG )
endif()
