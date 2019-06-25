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

# set(CMAKE_BUILD_TYPE "Debug")
# set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
# set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

project(PossionRec)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH CMAKE_MODULE_PATH "/home/yucheng/Software/pcl-master/cmake/Modules")

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

#file(COPY config	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#file(COPY calibration	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#file(COPY output	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#file(COPY resources     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
#file(COPY src/DLP/DLPR350PROM-3.1.0	DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#file(COPY config	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
#file(COPY calibration	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
#file(COPY output	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
#file(COPY resources     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
#file(COPY src/DLP/DLPR350PROM-3.1.0	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    )

# ---[ VTK
# set(CMAKE_PREFIX_PATH /home/yucheng/Software/VTK-8.1.1/build)
#if(NOT DEFINED ENV{VTK_BUILD_DIR})
#    message(FATAL_ERROR "not defined environment variable:VTK_BUILD_DIR
#        Please define VTK_BUILD_DIR in /etc/environment and restart the computer
#        e.g. export VTK_BUILD_DIR=/home/yucheng/Software/VTK-8.1.1/build")
#endif()
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
list(APPEND _LINKER_LIBS ${VTK_LIBRARIES})
#message(STATUS "VTK found (${VTK_CMAKE_DIR})")
add_definitions(-DUSE_VTK)

# ---[ PCL
# set(CMAKE_PREFIX_PATH /home/yucheng/Software/pcl-master/build)
#if(NOT DEFINED ENV{PCL_BUILD_DIR})
#    message(FATAL_ERROR "not defined environment variable:PCL_BUILD_DIR
#        Please define PCL_BUILD_DIR in /etc/environment and restart the computer
#        e.g. export PCL_BUILD_DIR=/home/yucheng/Software/pcl/build")
#endif()
find_package(PCL 1.8 REQUIRED)
include_directories(SYSTEM ${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})
list(APPEND _LINKER_LIBS ${PCL_LIBRARIES})
add_definitions(-DUSE_PCL)
#message(STATUS "PCL found (${PCL_CMAKE_DIR})")

# ---[ OpenCV
# if not OpenCV 3.x, then imgcodecs are not found
# set(CMAKE_PREFIX_PATH /home/yucheng/Software/opencv-3.4.2/build)
#if(NOT DEFINED ENV{OPENCV_BUILD_DIR})
#    message(FATAL_ERROR "not defined environment variable:OPENCV_BUILD_DIR
#        Please define OPENCV_BUILD_DIR in /etc/environment and restart the computer
#        e.g. export OPENCV_BUILD_DIR=/home/yucheng/Software/opencv-3.4.2/build")
#endif()
find_package(OpenCV QUIET COMPONENTS core highgui features2d calib3d flann imgproc imgcodecs plot)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
list(APPEND _LINKER_LIBS ${OpenCV_LIBS})
#message(STATUS "OpenCV found (${OpenCV_CONFIG_PATH})")
add_definitions(-DUSE_OPENCV)

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
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

#MESSAGE("${CMAKE_PREFIX_PATH}")
# Find the QtWidgets library
set(CMAKE_PREFIX_PATH /home/yucheng/Software/QT5.11.1/5.11.1/gcc_64/lib/cmake)
#if(NOT DEFINED ENV{QT_LIB_CMAKE})
#    message(FATAL_ERROR "not defined environment variable:QT_LIB_CMAKE
#        Please define QT_LIB_CMAKE in /etc/environment and restart the computer
#        e.g. export QT_LIB_CMAKE=/home/yucheng/Software/QT5.11.1/5.11.1/gcc_64/lib/cmake")
#endif()
find_package(Qt5Widgets)
qt5_wrap_ui(outfiles src/mainwindow.ui)
add_executable(${PROJECT_NAME} ${_SOURCES} ${_HEADERS} ${outfiles})

#target_link_libraries(${PROJECT_NAME} ${DLP_SDK_LIBRARIES})

# Use the Widgets module from Qt 5.
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Possion
    PNG
    JPEG
    ZLIB
    ${_LINKER_LIBS})


