#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(FMT_INSTALL OFF CACHE BOOL "" FORCE)
set(FMT_OS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(fmt GIT_REPOSITORY "https://github.com/fmtlib/fmt" GIT_TAG "12.1.0")
FetchContent_MakeAvailable(fmt)

set_target_properties(fmt PROPERTIES FOLDER deps)
