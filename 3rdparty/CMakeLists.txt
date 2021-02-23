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

add_library(llhttp STATIC llhttp/api.c llhttp/http.c llhttp/llhttp.c llhttp/llhttp.h)
set_property(TARGET llhttp PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(multipart STATIC multipart/multipartparser.c multipart/multipartparser.h)
set_property(TARGET multipart PROPERTY POSITION_INDEPENDENT_CODE ON)
