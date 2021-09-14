#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

add_library(llhttp STATIC llhttp/api.c llhttp/http.c llhttp/llhttp.c llhttp/llhttp.h)
add_library(multipart STATIC multipart/multipartparser.c multipart/multipartparser.h)
