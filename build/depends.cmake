# C/C++ settings
IF( ${CMAKE_CXX_COMPILER} MATCHES  "[cg][+][+]")
    MESSAGE("G++ ")
    # -Wno-unused-parameter -Wno-unused-local-typedef -fheinous-gnu-extensions
    set(COMMON_FLAGS " -Wno-unused-parameter")
    set(OpenMP_FLAGS, " -fopenmp -fopenmp-simd -ftree-vectorizer-verbose=1 -ftree-vectorize")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -std=c++11 -g3")
    set(CMAKE_CXX_FLAGS_DEBUG "-g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    # -malign-double
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -march=native -mtune=native")

    set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -std=gnu99")
    set(CMAKE_C_FLAGS_DEBUG "-g3")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
    # -malign-double
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} -march=native -mtune=native")

    #add_definitions(-D_BSD_SOURCE)
    #add_definitions(-D__USE_POSIX199309)
    #add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11> $<$<COMPILE_LANGUAGE:C>:-std=gnu99>)
ELSE()

ENDIF()

# Shared lib extention
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #shared lib suffix
    set(dll_ext ".so")
ELSEIF (MATCHES "Darwin")
    set(dll_ext ".dylib")
ELSEIF (MATCHES "Windows")
    set(dll_ext ".dll")
ENDIF()

# Darwin RPATH
IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    #Set osx rpath
    set( CMAKE_MACOSX_RPATH 1)
    set( MACOSX_RPATH "@executable_path")
ENDIF()

# OpenCL
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    find_package(OpenCL)
    if(OPENCL_FOUND)
        INCLUDE_DIRECTORIES(${OpenCL_INCLUDE_DIRS} /usr/local/include)
        LINK_DIRECTORIES(${OpenCL_LIBRARY})
    else()
        #OpenCL cuda7.5 version
        set(OpenCL_DIR /usr/local/cuda-7.5)
        set(OpenCL_LIBRARIES OpenCL)
        INCLUDE_DIRECTORIES(${OpenCL_DIR}/include)
        LINK_DIRECTORIES(${OpenCL_DIR}/lib64)
    endif()

ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    #OpenCL
    find_package(OpenCL)
    if(OPENCL_FOUND)
        INCLUDE_DIRECTORIES(${OpenCL_INCLUDE_DIRS} /usr/local/include)
        LINK_DIRECTORIES(${OpenCL_LIBRARY})
    else()
        WARNING("Not found OpenCL !")
    endif()

ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

ENDIF()

# OpenMP
FIND_PACKAGE( OpenMP)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Boost
FIND_PACKAGE( Boost )
if(BOOST_FOUND)
    set(CMAKE_CXX_FLAGS "{CMAKE_CXX_FLAGS} ${Boost_CXX_FLAGS}")
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${OpenCL_LIBRARY})
endif()
