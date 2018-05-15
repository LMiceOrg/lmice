cmake_minimum_required(VERSION 3.1)

project(futuremodel)


IF(${eal_header} MATCHES "")
##LIB EAL
FIND_PATH(eal_header lmice_trace.h
    PATHS ../fmtrader/eal)
ENDIF()

INCLUDE_DIRECTORIES(${eal_header}/..)

# CMake settings
set(CMAKE_VERBOSE_MAKEFILE on)

file(GLOB_RECURSE libfuturemodel_src RELATIVE_PATH include "*.h")

list(APPEND libfuturemodel_src
    rollscheme.cpp
    cu1_0_forecaster.cpp
    )

INCLUDE_DIRECTORIES(.)

ADD_LIBRARY(libfuturemodel STATIC ${libfuturemodel_src})
SET_TARGET_PROPERTIES(libfuturemodel PROPERTIES OUTPUT_NAME "futuremodel")
SET_TARGET_PROPERTIES(libfuturemodel PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(libfuturemodel PROPERTIES VERSION 1.1 SOVERSION 1)

file(GLOB_RECURSE libc_futuremodel_src RELATIVE_PATH include "*.h")

list(APPEND libc_futuremodel_src
    portfolio.cpp
)

ADD_LIBRARY(libc_futuremodel STATIC ${libc_futuremodel_src})
SET_TARGET_PROPERTIES(libc_futuremodel PROPERTIES OUTPUT_NAME "cfuturemodel")
SET_TARGET_PROPERTIES(libc_futuremodel PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(libfuturemodel PROPERTIES VERSION 1.1 SOVERSION 1)


set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")

#ADD_LIBRARY(futuremodel SHARED ${libfuturemodel_src})
#TARGET_LINK_LIBRARIES(futuremodel ${eal_static})
