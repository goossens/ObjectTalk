#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB OT_INCLUDES "*.h" "ot/*.h")
add_custom_target(includes SOURCES ${OT_INCLUDES})
