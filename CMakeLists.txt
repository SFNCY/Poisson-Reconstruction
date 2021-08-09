cmake_minimum_required(VERSION 3.3)

################################################################################################
# Short command for setting defeault target properties
# Usage:
#   default_properties(<target>)
function(default_properties target)
  set_target_properties(${target} PROPERTIES
    DEBUG_POSTFIX "-d"
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
  # make sure we build all external depepdencies first
  if (DEFINED external_project_dependencies)
    add_dependencies(${target} ${external_project_dependencies})
  endif()
endfunction()

file(GLOB _SOURCES "src/*.c" "src/*.cpp")
file(GLOB _HEADERS "src/*.h")

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

project(PossionRec)

# Ensure C++11 compatibility
if(NOT MSVC)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()
endif(NOT MSVC )

#Set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    )

# ---[ PCL
# set(CMAKE_PREFIX_PATH /home/yucheng/Software/pcl-master/build)
#if(NOT DEFINED ENV{PCL_BUILD_DIR})
#    message(FATAL_ERROR "not defined environment variable:PCL_BUILD_DIR
#        Please define PCL_BUILD_DIR in /etc/environment and restart the computer
#        e.g. export PCL_BUILD_DIR=/home/yucheng/Software/pcl/build")
#endif()
find_package(PCL 1.9 REQUIRED)
include_directories(SYSTEM ${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
list(APPEND _LINKER_LIBS ${PCL_LIBRARIES})
add_definitions(-DUSE_PCL)
#message(STATUS "PCL found (${PCL_CMAKE_DIR})")

set(POSSION_INCLUDE_DIRS
    )
set(POSSION_LIBS
    )

add_subdirectory(src)
#message(STATUS "Possion path:" ${POSSION_INCLUDE_DIRS})
include_directories(SYSTEM  ${POSSION_INCLUDE_DIRS})
include_directories(SYSTEM  ${PNG_INCLUDE_DIRS})
include_directories(SYSTEM  ${JPEG_INCLUDE_DIRS})
include_directories(SYSTEM  ${ZLIB_INCLUDE_DIRS})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} ${_SOURCES} ${_HEADERS} )

target_link_libraries(${PROJECT_NAME}
    Possion
    PNG
    JPEG
    ZLIB
    ${_LINKER_LIBS})


