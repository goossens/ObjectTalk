//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

SAMPLER2D(s_postProcessTexture, 0);
SAMPLER2D(s_bloomTexture, 1);

void main() {
	// sample color
	vec3 color = texture2D(s_postProcessTexture, v_texcoord0).rgb;
	color += texture2D(s_bloomTexture, v_texcoord0).rgb;
	gl_FragColor = vec4(color, 1.0);
}
