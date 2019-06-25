file(GLOB SRCS "*.c" "*.cpp")
file(GLOB HEADS "*.h" "*.hpp" "*.inl")

list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${INCLUDE_DIRS})

set(PNG_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

add_library(PNG ${SRCS})

default_properties(PNG)
