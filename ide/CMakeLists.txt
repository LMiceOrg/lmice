project(ide)

include(../build/precompiledheader.cmake)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Network Widgets OpenGL Charts)

find_package(PythonInterp 2.7 REQUIRED
    )
find_package(PythonLibs 2.7 REQUIRED
    )


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
qt5_wrap_ui(ui_files candlesticksetting.ui)
#qt5_wrap_cpp(moc_files mainwindow.cpp)
#qt5_add_resources(qrc_files drawable.qrc)

LIST(APPEND lmice_files main.cpp
    graphicsview.cpp graphicsview.h
    graphicswidget.cpp graphicswidget.h
    twosidedgraphicswidget.cpp twosidedgraphicswidget.h
    candlestickchart.cpp candlestickchart.h
    candlesticksetting.cpp candlesticksetting.ui candlesticksetting.h
    highlighter.cpp highlighter.h
    guava2loader.cpp guava2loader.h
    stdafx.cpp stdafx.h
    pythonembed.c pythonembed.h
    pythonhelper.cpp    pythonhelper.h
    guavaproto.h
    chinal1source.c
    instrumentfilter.c
    redirectio.c
    windowproxy.c
    loggingoutput.cpp loggingoutput.h
    loggingmodel.cpp loggingmodel.h
    ../../fmtrader/eal/lmice_trace.c ../../fmtrader/eal/lmice_trace.h
    ${ui_files} ${moc_files})

add_executable(lmice ${lmice_files})

add_precompiled_header(lmice stdafx.h SOURCE_CXX stdafx.cpp FORCEINCLUDE)
qt5_use_modules(lmice Core Widgets Network Gui OpenGL Charts)
target_link_libraries(lmice ${PYTHON_LIBRARIES})


message(STATUS "PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
message(STATUS "PYTHON_EXECUTABLE = ${PYTHON_EXECUTABLE}")
message(STATUS "PYTHON_INCLUDE_DIRS = ${PYTHON_INCLUDE_DIRS}")

include_directories(${PYTHON_INCLUDE_DIRS} ../../fmtrader/eal)

# Copy files to bin
install( DIRECTORY "pymodule" DESTINATION "${EXECUTABLE_OUTPUT_PATH}" FILES_MATCHING PATTERN "pymodule/*.*")

#LIST(APPEND pymodule_files pymodule/__init__.py)
file(GLOB_RECURSE pymodule_files pymodule, "*.py")
#AUX_SOURCE_DIRECTORY(pymodule pymodule_files)
add_custom_target(lmice_pymodule_files ALL SOURCES ${pymodule_files})

