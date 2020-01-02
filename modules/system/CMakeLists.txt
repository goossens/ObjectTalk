find_library(UV_LIBRARY uv)

add_library(system MODULE system.cpp)
target_compile_features(system PRIVATE cxx_std_17)
set_target_properties(system PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(system PRIVATE ${UV_LIBRARY})
