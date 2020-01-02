add_executable(ot ot.cpp)

target_compile_features(ot PRIVATE cxx_std_17)
set_target_properties(ot PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(ot PRIVATE m)
