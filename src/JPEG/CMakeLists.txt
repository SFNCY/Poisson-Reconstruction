file(GLOB SRCS "*.c" "*.cpp")
file(GLOB HEADS "*.h" "*.hpp" "*.inl")

list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIRS})

set(JPEG_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

add_library(JPEG ${SRCS})

default_properties(JPEG)
