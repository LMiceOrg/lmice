cmake_minimum_required(VERSION 3.1)

project(service)

INCLUDE_DIRECTORIES(. ${eal_header} ${eal_header}/.. ../trader)

file(GLOB_RECURSE service_include_h RELATIVE_PATH include "*.h")

LIST(APPEND service_files
    ${service_include_h}
    service_logging.cpp
    worker_board.cpp

)

ADD_LIBRARY(service_static STATIC ${service_files})

SET_TARGET_PROPERTIES(service_static PROPERTIES OUTPUT_NAME "service")
SET_TARGET_PROPERTIES(service_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)


LIST(APPEND service_server_files
    ${service_include_h}
    lmiced.c
    uds.c
    #server.c
    #execution_manager.c
    resource.c
    res_manager.c
)

ADD_EXECUTABLE(service_server ${service_server_files})
SET_TARGET_PROPERTIES(service_server PROPERTIES OUTPUT_NAME "lmiced")
target_link_libraries(service_server ${jansson_libs} eal_static)
