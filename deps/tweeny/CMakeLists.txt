#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Declare(tweeny GIT_REPOSITORY "https://github.com/mobius3/tweeny")
FetchContent_MakeAvailable(tweeny)
set_target_properties(tweeny-dummy PROPERTIES FOLDER deps)
