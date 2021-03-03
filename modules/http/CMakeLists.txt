#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

find_path(UV_INCLUDES uv.h REQUIRED)
find_library(UV_LIBRARY uv REQUIRED)

add_library(http MODULE http.cpp)
target_compile_features(http PRIVATE cxx_std_17)
set_target_properties(http PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(http PRIVATE ${UV_INCLUDES})
target_link_libraries(http PRIVATE ${UV_LIBRARY} llhttp multipart)

install(TARGETS http DESTINATION lib/ot)
