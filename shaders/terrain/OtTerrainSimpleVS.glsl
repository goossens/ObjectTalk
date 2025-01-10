//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_texcoord0

#include <bgfx_shader.glsl>

#include <OtTerrainUniforms.glsl>

// functions
void main() {
	// determine heightmap coordinates
	vec2 uv = mul(u_model[0], vec4(a_position, 1.0)).xz * u_hScale / u_heightMapSize;

	// determine height and normal
	vec4 sample = texture2DLod(s_normalmapTexture, uv, 1);
	float height = sample.w;

	// determine world position
	vec3 position = mul(u_model[0], vec4(a_position.x, height, a_position.z, 1.0)).xyz;

	// generate output
	v_texcoord0 = uv;
	gl_Position = mul(u_viewProj, vec4(position, 1.0));
}
