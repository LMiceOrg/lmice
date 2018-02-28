cmake_minimum_required(VERSION 3.1)

project(lmice)

include(build/depends.cmake)

# CMake settings
set(CMAKE_VERBOSE_MAKEFILE off)


add_custom_target(doc ALL SOURCES LICENSE README.md .gitignore)


add_subdirectory(ide)
