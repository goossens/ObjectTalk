#	ObjectTalk Scripting Language
#	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
endif()

foreach(GLSL ${SHADER_GLSL})
	getStageName(SHADER_STAGE ${GLSL})
	set(TARGET_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/generated/${SHADER_STAGE}.cpp)

	if(NOT WIN32)
		add_custom_command(
			OUTPUT ${TARGET_SOURCE}
			DEPENDS ${GLSL}
			COMMAND ${GLSL_VALIDATOR} -I${CMAKE_CURRENT_SOURCE_DIR}/library -V ${GLSL} --vn ${SHADER_STAGE} -o ${TARGET_SOURCE}.tmp1
			COMMAND ${SED_EXECUTABLE} -f ${CMAKE_CURRENT_SOURCE_DIR}/templates/shader.sed ${TARGET_SOURCE}.tmp1 > ${TARGET_SOURCE}.tmp2
			COMMAND ${CMAKE_COMMAND} -E cat ${CMAKE_CURRENT_SOURCE_DIR}/templates/header.cpp ${TARGET_SOURCE}.tmp2 > ${TARGET_SOURCE}
			COMMAND ${SED_EXECUTABLE} "s/SHADER/${SHADER_STAGE}/g" ${CMAKE_CURRENT_SOURCE_DIR}/templates/footer.cpp >> ${TARGET_SOURCE}
			COMMAND ${CMAKE_COMMAND} -E remove -f ${TARGET_SOURCE}.tmp1 ${TARGET_SOURCE}.tmp2
		)
	endif()

	list(APPEND SHADER_CPP ${TARGET_SOURCE})
endforeach(GLSL)

if(NOT WIN32)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/OtShaders.h
		DEPENDS ${SHADER_GLSL}
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/templates/shaders.sh
	)
endif()

add_library(shaders STATIC ${CMAKE_CURRENT_SOURCE_DIR}/OtShaders.h ${SHADER_CPP} ${SHADER_GLSL})
target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${CMAKE_CURRENT_SOURCE_DIR}/OtShaders.h ${SHADER_CPP} ${SHADER_GLSL})
