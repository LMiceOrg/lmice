project(ide)

FIND_PACKAGE(Qt5 REQUIRED Core Gui Network Widgets OpenGL Charts)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}")
qt5_wrap_ui(ui_files candlesticksetting.ui)
#qt5_wrap_cpp(moc_files mainwindow.cpp)
#qt5_add_resources(qrc_files drawable.qrc)

LIST(APPEND ide_files main.cpp
    graphicsview.cpp graphicsview.h
    graphicswidget.cpp graphicswidget.h
    twosidedgraphicswidget.cpp twosidedgraphicswidget.h
    candlestickchart.cpp candlestickchart.h
    candlesticksetting.cpp candlesticksetting.ui candlesticksetting.h
    highlighter.cpp highlighter.h
    guava2loader.cpp guava2loader.h
    ${ui_files} ${moc_files})
add_executable(lmice ${ide_files})
qt5_use_modules(lmice Core Widgets Network Gui OpenGL Charts)

