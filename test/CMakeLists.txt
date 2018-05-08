cmake_minimum_required(VERSION 3.1)

project(unittest)


IF(${eal_header} MATCHES "")
##LIB EAL
FIND_PATH(eal_header lmice_trace.h
    PATHS ../fmtrader/eal)
ENDIF()

INCLUDE_DIRECTORIES(${eal_header}/..)

# CMake settings
set(CMAKE_VERBOSE_MAKEFILE on)


list(APPEND testfuturemodel_src
    testfuturemodel.cpp
    )

INCLUDE_DIRECTORIES(. ${eal_header} ../futuremodel)

add_executable(testfm ${testfuturemodel_src})

set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")

TARGET_LINK_LIBRARIES(testfm ${eal_static} ${libfuturemodel})
