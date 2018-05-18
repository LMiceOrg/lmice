cmake_minimum_required(VERSION 3.1)

project(trader)


IF(${eal_header} MATCHES "")

##LIB EAL
FIND_PATH(eal_header lmice_trace.h
    PATHS ../fmtrader/eal)

INCLUDE_DIRECTORIES(${eal_header}/.. ${eal_header})

ENDIF()


file(GLOB_RECURSE trader_xone_emulator_src RELATIVE_PATH xone/include "*.h")
file(GLOB_RECURSE trader_xone_emulator_src RELATIVE_PATH include "*.h")
list(APPEND trader_xone_emulator_src
    trader_xone_emulator.cpp
    trader_xone_spi.cpp
    )

INCLUDE_DIRECTORIES(.)

#add_executable( trader_xone ${trader_xone_src})
#TARGET_LINK_LIBRARIES(trader_xone futuremodel ${eal_static})



set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")

ADD_LIBRARY(lib_trader_xone_emulator SHARED ${trader_xone_emulator_src})
SET_TARGET_PROPERTIES(lib_trader_xone_emulator PROPERTIES OUTPUT_NAME "xone_emulator")
SET_TARGET_PROPERTIES(lib_trader_xone_emulator PROPERTIES CLEAN_DIRECT_OUTPUT 1)
TARGET_LINK_LIBRARIES(lib_trader_xone_emulator libc_futuremodel eal_static)
