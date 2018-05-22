cmake_minimum_required(VERSION 3.1)

project(trader)


IF(${eal_header} MATCHES "")

##LIB EAL
FIND_PATH(eal_header lmice_trace.h
    PATHS ../fmtrader/eal)

INCLUDE_DIRECTORIES(${eal_header}/.. ${eal_header})

ENDIF()


file(GLOB_RECURSE trader_xone_emulator_h RELATIVE_PATH  "xone/*.h")
file(GLOB_RECURSE trader_h RELATIVE_PATH  "include/*.h")
list(APPEND trader_xone_emulator_src
    ${trader_xone_emulator_h}
    ${trader_h}
    trader_xone_emulator.cpp
    trader_xone_spi.cpp
    )

INCLUDE_DIRECTORIES(. ../futuremodel ../service)




set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")

ADD_LIBRARY(lib_trader_xone_emulator STATIC ${trader_xone_emulator_src})
SET_TARGET_PROPERTIES(lib_trader_xone_emulator PROPERTIES OUTPUT_NAME "xone_emulator")
#SET_TARGET_PROPERTIES(lib_trader_xone_emulator PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#TARGET_LINK_LIBRARIES(lib_trader_xone_emulator libc_futuremodel eal_static service_static)

#Xone unit test
list(APPEND app_test_xone_emulator_src
    ${trader_xone_emulator_h}
    ${trader_h}
    ../test/testxoneemulator.cpp
    )
add_executable( test_xone_emulator ${app_test_xone_emulator_src})
TARGET_LINK_LIBRARIES(test_xone_emulator ${jansson_libs} libc_futuremodel eal_static  lib_trader_xone_emulator service_static)

# FemasAPI V2
file(GLOB_RECURSE trader_femas2_h RELATIVE_PATH  "femas2/*.h")
list(APPEND trader_femas2_emulator_src
    ${trader_femas2_h}
    ${trader_h}
    trader_femas2_emulator.cpp
    trader_femas2_spi.cpp
    )

ADD_LIBRARY(lib_trader_femas2_emulator STATIC ${trader_femas2_emulator_src})
SET_TARGET_PROPERTIES(lib_trader_femas2_emulator PROPERTIES OUTPUT_NAME "femas2_emulator")

# FemasAPI V2 unit test
list(APPEND app_test_femas2_emulator_src
    ${trader_femas2_h}
    ${trader_h}
    ../test/testfemas2emulator.cpp
    )
add_executable( test_femas2_emulator ${app_test_femas2_emulator_src})
TARGET_LINK_LIBRARIES(test_femas2_emulator ${jansson_libs} libc_futuremodel   lib_trader_femas2_emulator eal_static service_static)
