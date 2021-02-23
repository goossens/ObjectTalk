#	ObjectTalk Scripting Language
#	Copyright 1993-2021 Johan A. Goossens
#
#	Licensed under the Apache License, Version 2.0 (the "License");
#	you may not use this file except in compliance with the License.
#	You may obtain a copy of the License at
#
#		http:#www.apache.org/licenses/LICENSE-2.0
#
#	Unless required by applicable law or agreed to in writing, software
#	distributed under the License is distributed on an "AS IS" BASIS,
#	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#	See the License for the specific language governing permissions and
#	limitations under the License.

find_path(UV_INCLUDES uv.h)
find_library(UV_LIBRARY uv)

add_library(net MODULE net.cpp)
target_compile_features(net PRIVATE cxx_std_17)
set_target_properties(net PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(net PRIVATE ${UV_INCLUDES})
target_link_libraries(net PRIVATE ${UV_LIBRARY} llhttp multipart)

install(TARGETS net DESTINATION lib/ot)
