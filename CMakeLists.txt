cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ObjectTalk VERSION 1.0 DESCRIPTION "ObjectTalk - The Completely Object-Oriented Scripting Language" LANGUAGES CXX)

include(GNUInstallDirs)

include_directories(include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SHARED_MODULE_PREFIX "")

enable_testing()

add_subdirectory(app)
add_subdirectory(modules)
add_subdirectory(test)
