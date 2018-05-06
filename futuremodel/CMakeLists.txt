cmake_minimum_required(VERSION 3.1)

project(model)


FIND_LIBRARY(eal_libs NAMES eal
                PATHS ${CMAKE_BINARY_DIR}
            )
FIND_PATH(eal_header lmice_trace.h PATHS ../../fmtrader/eal)
INCLUDE_DIRECTORIES(${eal_header}/..)

# CMake settings
set(CMAKE_VERBOSE_MAKEFILE on)


list(APPEND libfuturemodel_src
    scheme.h
    rollscheme.h rollscheme.cpp
    chinal1msg.h fastmath.h
    )


ADD_LIBRARY(libfuturemodel STATIC ${libfuturemodel_src})
SET_TARGET_PROPERTIES(libfuturemodel PROPERTIES OUTPUT_NAME "futuremodel")
SET_TARGET_PROPERTIES(libfuturemodel PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(libfuturemodel PROPERTIES VERSION 1.1 SOVERSION 1)
set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")

ADD_LIBRARY(futuremodel SHARED ${libfuturemodel_src})
TARGET_LINK_LIBRARIES(futuremodel ${eal_libs})
