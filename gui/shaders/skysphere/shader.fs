//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

// uniforms
uniform vec4 u_background[3];
#define u_brightness u_background[0].x
#define u_gamma u_background[0].y

SAMPLER2D(s_skySphereTexture, 0);

void main() {
	vec3 color = texture2D(s_skySphereTexture, v_texcoord0).rgb;
	color = pow(color, vec3_splat(u_gamma));
	color = u_brightness * color;
	gl_FragColor = vec4(color, 1.0f);
}
