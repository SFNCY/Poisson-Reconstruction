file(GLOB SRCS "*.cpp")
file(GLOB HEADS "*.h" "*.hpp" "*.inl")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIRS})

set(POSSION_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

add_library(Possion ${SRCS})

default_properties(Possion)
