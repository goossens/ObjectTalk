find_library(UV_LIBRARY uv)

add_library(net MODULE net.cpp)
target_compile_features(net PRIVATE cxx_std_17)
set_target_properties(net PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(net PRIVATE ${UV_LIBRARY})
