#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

function(getGuiDefault)
	if (APPLE)
		set(OT_GUI_DEFAULT ON PARENT_SCOPE)
	endif()

	if (UNIX AND NOT APPLE)
		# for Linux, BSD, Solaris, Minix
		set(OT_GUI_DEFAULT OFF PARENT_SCOPE)
	endif()

	if (WIN32)
		set(OT_GUI_DEFAULT ON PARENT_SCOPE)
	endif()

endfunction()
