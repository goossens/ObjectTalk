//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <albedo.glsl>

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

	gl_FragColor = vec4_splat(1.0);
}
