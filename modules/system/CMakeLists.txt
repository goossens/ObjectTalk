#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

find_path(UV_INCLUDES uv.h)
find_library(UV_LIBRARY uv)

add_library(system MODULE system.cpp)
target_compile_features(system PRIVATE cxx_std_17)
set_target_properties(system PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(system PRIVATE ${UV_INCLUDES})
target_link_libraries(system PRIVATE ${UV_LIBRARY})

install(TARGETS system DESTINATION lib/ot)
