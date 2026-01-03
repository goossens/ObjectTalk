#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE OT_EXAMPLES "*.ot")

add_custom_target(examples SOURCES ${OT_EXAMPLES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${OT_EXAMPLES})
