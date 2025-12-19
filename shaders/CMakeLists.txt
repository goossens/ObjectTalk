#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE SHADER_GLSL CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/*/*.vert
	${CMAKE_CURRENT_SOURCE_DIR}/*/*.frag
	${CMAKE_CURRENT_SOURCE_DIR}/*/*.comp)

function(getStageName stageName fileName)
	get_filename_component(FILENAME ${fileName} NAME_WLE)
	get_filename_component(EXTENSION ${fileName} LAST_EXT)

	string(TOUPPER "${EXTENSION}" U)
	string(SUBSTRING "${U}" 1  1 A)
	string(SUBSTRING "${EXTENSION}" 2 -1 B)

	set(${stageName} "${FILENAME}${A}${B}" PARENT_SCOPE)
endfunction()

if(NOT WIN32)
	find_program(GLSL_VALIDATOR "glslangValidator" REQUIRED)
	find_program(SED_EXECUTABLE "sed" REQUIRED)

	execute_process(
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/templates/shaders.h ${CMAKE_CURRENT_SOURCE_DIR}/OtShaders.h
	)
endif()

foreach(GLSL ${SHADER_GLSL})
	getStageName(SHADER_STAGE ${GLSL})
	set(TARGET_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/generated/${SHADER_STAGE}.h)
	set(TARGET_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/generated/${SHADER_STAGE}.cpp)

	if(NOT WIN32)
		execute_process(
			COMMAND sh -c "echo 'extern const uint32_t ${SHADER_STAGE}[];' >>${CMAKE_CURRENT_SOURCE_DIR}/OtShaders.h"
			COMMAND sh -c "echo 'extern const size_t ${SHADER_STAGE}Size;' >>${CMAKE_CURRENT_SOURCE_DIR}/OtShaders.h"
		)

		add_custom_command(
			OUTPUT ${TARGET_INCLUDE}
			DEPENDS ${GLSL}
			COMMAND ${GLSL_VALIDATOR} -I${CMAKE_CURRENT_SOURCE_DIR}/library -V ${GLSL} --vn ${SHADER_STAGE} -o ${TARGET_INCLUDE}.tmp1
			COMMAND ${SED_EXECUTABLE} "s/const uint32_t/extern const uint32_t/g" ${TARGET_INCLUDE}.tmp1 > ${TARGET_INCLUDE}.tmp2
			COMMAND ${CMAKE_COMMAND} -E cat ${CMAKE_CURRENT_SOURCE_DIR}/templates/include.h ${TARGET_INCLUDE}.tmp2 > ${TARGET_INCLUDE}
			COMMAND ${CMAKE_COMMAND} -E remove -f ${TARGET_INCLUDE}.tmp1 ${TARGET_INCLUDE}.tmp2
		)

		add_custom_command(
			OUTPUT ${TARGET_SOURCE}
			DEPENDS ${GLSL}
			COMMAND ${SED_EXECUTABLE} "s/SHADER/${SHADER_STAGE}/g" ${CMAKE_CURRENT_SOURCE_DIR}/templates/source.cpp > ${TARGET_SOURCE}
		)
	endif()

	list(APPEND SHADER_H ${TARGET_INCLUDE})
	list(APPEND SHADER_CPP ${TARGET_SOURCE})
endforeach(GLSL)

add_library(shaders STATIC ${SHADER_CPP} ${SHADER_H} ${SHADER_GLSL})
target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SHADER_GLSL} ${SHADER_CPP})
