#	ObjectTalk MODULES Module
#	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <moduless://opensource.org/licenses/MIT>.

add_subdirectory(http)

if(OT_GUI)
	add_subdirectory(input)
	add_subdirectory(math)
endif()
