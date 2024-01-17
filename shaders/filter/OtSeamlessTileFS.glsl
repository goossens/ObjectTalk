//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

SAMPLER2D(s_texture, 0);

void main() {
	vec2 uv2 = vec2(fract(v_texcoord0 + 0.5));
	float dist = distance(uv2, vec2(0.5, 0.5));
	float alpha = clamp((dist - 0.3) / (0.5 - 0.3), 0.0, 1.0);

	vec3 color1 = texture2D(s_texture, v_texcoord0).xyz;
	vec3 color2 = texture2D(s_texture, uv2).xyz;

	gl_FragColor = vec4(mix(color2, color1, alpha), 1.0);
//	gl_FragColor = vec4(vec3_splat(alpha), 1.0);
}
