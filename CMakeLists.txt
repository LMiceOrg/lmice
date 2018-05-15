cmake_minimum_required(VERSION 3.1)

project(lmice)

include(build/depends.cmake)
include(build/precompiledheader.cmake)

FIND_LIBRARY(jansson_libs NAMES jansson
                PATHS /usr/local /usr
            )
FIND_PATH(jansson_header jansson.h)
INCLUDE_DIRECTORIES(${jansson_header})

INCLUDE_DIRECTORIES(/Users/hehao/work/lib)

# CMake settings
set(CMAKE_VERBOSE_MAKEFILE on)

file(GLOB_RECURSE doc_files /Users/hehao/Downloads/doc/tech/pydocs-pdf/*.pdf)
list(APPEND doc_files LICENSE README.md .gitignore)
add_custom_target(doc ALL SOURCES  ${doc_files})

##LIB EAL
FIND_PATH(eal_header lmice_trace.h
    PATHS ../fmtrader/eal)
LIST(APPEND eal_SRCS
    #../fmtrader/eal/lmice_ring.c
    ../fmtrader/eal/lmice_trace.c
    ../fmtrader/eal/lmice_eal_shm.c
    ../fmtrader/eal/lmice_eal_hash.c
    ../fmtrader/eal/lmice_eal_spinlock.c
    ../fmtrader/eal/lmice_eal_malloc.c
    ../fmtrader/eal/lmice_eal_event.c
    ../fmtrader/eal/lmice_eal_time.c
    #../fmtrader/eal/lmice_core.c
    ../fmtrader/eal/lmice_eal_aio.c
    ../fmtrader/eal/lmice_eal_thread.c
    #../fmtrader/eal/lmice_bloomfilter.c
    )

#Shared EAL Library
#ADD_LIBRARY(eal SHARED ${EAL_SRCS})
#Static EAL Library
ADD_LIBRARY(eal_static STATIC ${eal_SRCS})

SET_TARGET_PROPERTIES(eal_static PROPERTIES OUTPUT_NAME "eal")
SET_TARGET_PROPERTIES(eal_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(eal_static PROPERTIES VERSION 1.2 SOVERSION 1)

INCLUDE_DIRECTORIES(${eal_header})


add_subdirectory(ide)

add_subdirectory(futuremodel)

add_subdirectory(test)


list(APPEND libforecaster_src
        ../lib/libforecaster/Feature.cpp
        ../lib/libforecaster/Feature.h
        ../lib/libforecaster/ChinaL1DiscreteFeature.cpp
        ../lib/libforecaster/ChinaL1DiscreteFeature.h
        ../lib/libforecaster/forecaster.cpp
        ../lib/libforecaster/forecaster.h
        ../lib/libforecaster/RollScheme.cpp
        ../lib/libforecaster/RollScheme.h
        ../lib/libforecaster/chinal1msg.h
        ../lib/libforecaster/scheme.h
    )

include_directories(../fmtrader)

ADD_LIBRARY(libforecaster STATIC ${libforecaster_src})
SET_TARGET_PROPERTIES(libforecaster PROPERTIES OUTPUT_NAME "forecaster")
SET_TARGET_PROPERTIES(libforecaster PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(libforecaster PROPERTIES VERSION 1.2 SOVERSION 1)

#file(GLOB_RECURSE libforecaster_pyfiles "${CMAKE_SOURCE_DIR}/../lib/libforecaster", "*.py")
#AUX_SOURCE_DIRECTORY(pymodule pymodule_files)
list(APPEND libforecaster_pyfiles ../lib/libforecaster/schemegen.py)
add_custom_target(libforecaster_pymodule ALL SOURCES ${libforecaster_pyfiles})
message( ${CMAKE_SOURCE_DIR}/../lib/libforecaster)

list(APPEND testforecaster_files
    ../lib/libforecaster/test.cpp
    ../fmtrader/eal/lmice_trace.c)

add_executable(testforecaster ${testforecaster_files})
target_link_libraries( testforecaster libforecaster_new ${jansson_libs} eal_static)

list(APPEND libforecaster_new_src
        ../lib/libforecaster/rollscheme.cpp
        ../lib/libforecaster/rollscheme.h
        ../lib/libforecaster/chinal1msg.h
        ../lib/libforecaster/scheme.h
        ../lib/libforecaster/forecasterbase.h ../lib/libforecaster/forecasterbase.cpp
        ../lib/libforecaster/featurebase.h ../lib/libforecaster/featurebase.cpp
        ../lib/libforecaster/chinal1discreteselftradeimbalancefeature.h ../lib/libforecaster/chinal1discreteselftradeimbalancefeature.cpp
        ../lib/libforecaster/chinal1discreteselftradeimbalancefeaturev2.h ../lib/libforecaster/chinal1discreteselftradeimbalancefeaturev2.cpp
        ../lib/libforecaster/chinal1discreteselfbookimbalancefeature.h ../lib/libforecaster/chinal1discreteselfbookimbalancefeature.cpp
        ../lib/libforecaster/chinal1discreteselforderflowimbalancefeature.h ../lib/libforecaster/chinal1discreteselforderflowimbalancefeature.cpp
        ../lib/libforecaster/chinal1discreteselfdecayingreturnfeature.h ../lib/libforecaster/chinal1discreteselfdecayingreturnfeature.cpp

        ../lib/libforecaster/chinal1discreteotherdecayingreturnfeature.h ../lib/libforecaster/chinal1discreteotherdecayingreturnfeature.cpp
        ../lib/libforecaster/chinal1discreteothertradeimbalancefeature.h ../lib/libforecaster/chinal1discreteothertradeimbalancefeature.cpp
        ../lib/libforecaster/chinal1discreteothertradeimbalancefeaturev2.h ../lib/libforecaster/chinal1discreteothertradeimbalancefeaturev2.cpp
        ../lib/libforecaster/chinal1discreteotherbestsincefeature.h ../lib/libforecaster/chinal1discreteotherbestsincefeature.cpp
        ../lib/libforecaster/chinal1discreteotherbookimbalancefeature.h ../lib/libforecaster/chinal1discreteotherbookimbalancefeature.cpp

        ../lib/libforecaster/forecastermp.h ../lib/libforecaster/forecastermp.cpp

        ../lib/libforecaster/variantfeature.h

    )
ADD_LIBRARY(libforecaster_new STATIC ${libforecaster_new_src})
SET_TARGET_PROPERTIES(libforecaster_new PROPERTIES OUTPUT_NAME "forecasternew")
SET_TARGET_PROPERTIES(libforecaster_new PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(libforecaster PROPERTIES VERSION 1.2 SOVERSION 1)


list(APPEND testfork_files
    ../lib/libforecaster/testfork.cpp
)

add_executable(testfork ${testfork_files})
target_link_libraries( testfork libforecaster_new ${jansson_libs} eal_static)
