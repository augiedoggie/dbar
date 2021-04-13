cmake_minimum_required(VERSION 3.19)

project(DBAR)

set(CMAKE_MODULE_PATH "${DBAR_SOURCE_DIR}/CMakeModules")

include(UseHaiku)

haiku_add_executable(dbar dbar.cpp dbar.rdef)

target_link_libraries(dbar be)

install(TARGETS dbar RUNTIME DESTINATION bin)
