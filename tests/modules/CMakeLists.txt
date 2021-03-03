#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB OT_TESTS "*.ot")

foreach(TEST ${OT_TESTS})
	add_test(NAME ${TEST} COMMAND ot ${TEST})
endforeach()
