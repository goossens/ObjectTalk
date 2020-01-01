add_executable(ot ot.cpp)

target_compile_features(ot PRIVATE cxx_std_17)
set_target_properties(ot PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(ot PRIVATE m)

if(CMAKE_BUILD_TYPE STREQUAL Release)
	add_custom_command(TARGET ot POST_BUILD COMMAND strip ${CMAKE_BINARY_DIR}/bin/ot COMMAND upx --brute ${CMAKE_BINARY_DIR}/bin/ot)
endif()
