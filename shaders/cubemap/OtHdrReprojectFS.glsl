//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position

#include <bgfx_shader.glsl>
#include <constants.glsl>

// uniforms
SAMPLER2D(s_equirectangularMap, 0);

// functions
vec2 viewToUv(vec3 v) {
	// convert view vector to uv coordinates
#if BGFX_SHADER_LANGUAGE_GLSL
	vec2 uv = vec2(atan2(v.x, v.z), asin(-v.y));
#else
	vec2 uv = vec2(atan2(v.x, v.z), asin(v.y));
#endif

	// constant to convert [-π, π] range to [-0.5, 0.5]
	const vec2 C = vec2(1.0 / TWO_PI, 1.0 / PI);

	// convert [-π, π] range to [0.0, 1.0]
	return uv * C + 0.5;
}

void main() {
	vec2 uv = viewToUv(normalize(v_position));
	vec3 color = texture2D(s_equirectangularMap, uv).rgb;
	gl_FragColor = vec4(color, 1.0f);
}
