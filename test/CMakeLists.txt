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

FIND_LIBRARY(eal_libs NAMES eal
    PATHS ${CMAKE_BINARY_DIR}
)

FIND_LIBRARY(fm_libs NAMES futuremodel
    PATHS ${CMAKE_BINARY_DIR}
)

#set(fm_libs ${CMAKE_BINARY_DIR}/libfuturemodel.a)

INCLUDE_DIRECTORIES(. ${eal_header} ../futuremodel)

add_executable(testfm ${testfuturemodel_src})



set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
message("fmlibs: ${fm_libs}")
TARGET_LINK_LIBRARIES(testfm  libfuturemodel eal_static)

add_executable(testfcnew testfuture2.cpp)
TARGET_LINK_LIBRARIES(testfcnew  libfuturemodel eal_static ${jansson_libs})


add_executable(testcmodel testcmodel.cpp)
TARGET_LINK_LIBRARIES(testcmodel  libc_futuremodel eal_static ${jansson_libs})
