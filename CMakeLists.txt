cmake_minimum_required(VERSION 3.1...3.14)
project(ObjectTalk VERSION 1.0 DESCRIPTION "Completely Object-Oriented Scripting Language" LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

file(GLOB OT_INCLUDES "include/*.h")

add_executable(ot app/main.cpp ${OT_INCLUDES})
target_include_directories(ot PRIVATE include)
target_link_libraries(ot PRIVATE m uv curl boost_filesystem boost_system)

if(CMAKE_BUILD_TYPE STREQUAL Release)
	add_custom_command(TARGET ot POST_BUILD COMMAND strip ot COMMAND upx --brute ot)
endif()

file(GLOB OT_TESTS "test/*.ot")

foreach(TEST ${OT_TESTS})
	add_test(${TEST} ot ${TEST})
endforeach()
