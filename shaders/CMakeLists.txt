#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
#
#	This work is licensed under the terms of the MIT license.
#	For a copy, see <https://opensource.org/licenses/MIT>.

file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
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
endif()

foreach(GLSL ${SHADER_SOURCES})
	getStageName(SHADER_STAGE ${GLSL})
	set(INCLUDE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/generated/${SHADER_STAGE}.h)

	if(NOT WIN32)
		add_custom_command(
			OUTPUT ${INCLUDE_FILE}
			COMMAND ${GLSL_VALIDATOR} -I${CMAKE_CURRENT_SOURCE_DIR}/library -V ${GLSL} --vn ${SHADER_STAGE} -o ${INCLUDE_FILE}.tmp
			COMMAND ${CMAKE_COMMAND} -E cat ${CMAKE_CURRENT_SOURCE_DIR}/OtShaderHeader.h ${INCLUDE_FILE}.tmp >${INCLUDE_FILE}
			COMMAND ${CMAKE_COMMAND} -E remove -f ${INCLUDE_FILE}.tmp
			DEPENDS ${GLSL})
	endif()

	list(APPEND SHADER_INCLUDE_FILES ${INCLUDE_FILE})
endforeach(GLSL)

add_library(shaders INTERFACE ${SHADER_SOURCES} ${SHADER_INCLUDE_FILES})
target_include_directories(shaders INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/generated)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SHADER_SOURCES} ${SHADER_INCLUDE_FILES})
