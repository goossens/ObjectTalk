//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_albedo[2];
#define u_color u_albedo[0]
#define u_hasTexture bool(u_albedo[1].x)
#define u_scale u_albedo[1].y

SAMPLER2D(s_albedoTexture, 0);

void main() {
	// determine albedo
	vec2 uv = v_texcoord0 * u_scale;
	vec4 albedo;

	if (u_hasTexture) {
		albedo = texture2D(s_albedoTexture, uv) * u_color;

	} else {
		albedo = u_color;
	}

	// discard pixel if too transparent
	if (albedo.a < 0.3) {
		discard;
	}

	gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
