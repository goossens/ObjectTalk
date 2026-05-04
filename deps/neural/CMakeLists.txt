#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

FetchContent_Populate(neural QUIET GIT_REPOSITORY "https://github.com/mikeoliphant/NeuralAudio" GIT_TAG "701ab02")
add_subdirectory(${neural_SOURCE_DIR} ${neural_BINARY_DIR} EXCLUDE_FROM_ALL)

set_target_properties(NeuralAudio PROPERTIES FOLDER deps)
set_target_properties(RTNeural PROPERTIES FOLDER deps)
set_target_properties(NeuralAudioCAPI PROPERTIES FOLDER deps)
