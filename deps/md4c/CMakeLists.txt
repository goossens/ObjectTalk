#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

set(BUILD_MD2HTML_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(md4c QUIET GIT_REPOSITORY "https://github.com/mity/md4c" GIT_TAG "release-0.5.3")
FetchContent_MakeAvailable(md4c)

set_target_properties(md4c PROPERTIES FOLDER deps)
set_target_properties(md4c-html PROPERTIES FOLDER deps/md4c)
