#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE OT_TESTS "*.ot")

foreach(TEST ${OT_TESTS})
	add_test(NAME ${TEST} COMMAND ${CMAKE_BINARY_DIR}/bin/ot ${TEST})
endforeach()

add_custom_target(tests SOURCES ${OT_TESTS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${OT_TESTS})
