//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_near, v_far

#include <bgfx_shader.glsl>

vec3 unproject(float x, float y, float z) {
	vec4 unprojectedPoint = mul(u_invViewProj, vec4(x, y, z, 1.0));
	return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
#if BGFX_SHADER_LANGUAGE_GLSL
	v_near = unproject(a_position.x, a_position.y, -1.0);
#else
	v_near = unproject(a_position.x, a_position.y, 0.0);
#endif

	v_far = unproject(a_position.x, a_position.y, 1.0);
	gl_Position = vec4(a_position, 1.0);
}
