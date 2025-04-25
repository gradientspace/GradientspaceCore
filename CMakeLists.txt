cmake_minimum_required(VERSION 3.19)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# this is set at end of script, allows add_directory() multiple times
if(DEFINED GSCORE_ADDED)
	return()
endif()

project(gradientspace_core)

#use C++20
set (CMAKE_CXX_STANDARD 20)

#todo support this
#option(GSCORE_USE_STATIC_LIBRARY "use GradientspaceCore as a static library")

# disable exceptions if desired
option(GSCORE_DISABLE_EXCEPTIONS "disable C++ Exceptions in GradientspaceCore library" OFF)
IF(GSCORE_DISABLE_EXCEPTIONS)
	add_definitions(-DGS_DISABLE_EXCEPTIONS)
ENDIF(GSCORE_DISABLE_EXCEPTIONS)

# what are these for?
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# find all files in top-level Private and Public source folders
file(GLOB_RECURSE PRIVATE_FILES DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.*")
file(GLOB_RECURSE PUBLIC_FILES DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Public/*.*")
set(MODULE_FILES ${PRIVATE_FILES} ${PUBLIC_FILES})

# configure the library
add_library(gradientspace_core SHARED ${MODULE_FILES})

# maintain directory tree in generated project files
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR})

# set include directories
target_include_directories(gradientspace_core PUBLIC "Public")
target_include_directories(gradientspace_core PUBLIC "Public/Color")
target_include_directories(gradientspace_core PUBLIC "Public/Core")
target_include_directories(gradientspace_core PUBLIC "Public/Grid")
target_include_directories(gradientspace_core PUBLIC "Public/Image")
target_include_directories(gradientspace_core PUBLIC "Public/Intersection")
target_include_directories(gradientspace_core PUBLIC "Public/Mapping")
target_include_directories(gradientspace_core PUBLIC "Public/Math")
target_include_directories(gradientspace_core PUBLIC "Public/Mesh")
target_include_directories(gradientspace_core PUBLIC "Public/Sampling")
target_include_directories(gradientspace_core PUBLIC "Public/Spatial")

# set up the GRADIENTSPACECORE_API macro...
target_compile_definitions(gradientspace_core PRIVATE GRADIENTSPACECORE_EXPORTS)

if(NOT PROJECT_IS_TOP_LEVEL)
	set(GSCORE_ADDED TRUE PARENT_SCOPE)
endif()
