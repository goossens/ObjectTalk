//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0, v_texcoord1

#include <bgfx_shader.glsl>

// texture samplers
SAMPLER2D(s_particlesTexture, 0);

// main function
void main() {
	vec2 uv1 = v_texcoord0.xy;
	vec2 uv2 = v_texcoord1.xy;
	float alpha = v_texcoord0.z;
	float blend = v_texcoord1.z;

	vec4 color1 = texture2D(s_particlesTexture, uv1);
	vec4 color2 = texture2D(s_particlesTexture, uv2);
	vec4 color = mix(color1, color2, blend);
	color.a *= alpha;

	if (color.a < 0.1) {
		discard;
	}

	gl_FragColor = vec4(color);
}
