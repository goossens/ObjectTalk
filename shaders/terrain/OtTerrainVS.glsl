//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position
$output v_position, v_normal, v_texcoord0

#include <bgfx_shader.glsl>
#include <OtTerrainUniforms.glsl>

// functions
void main() {
	// determine heightmap coordinates
	vec2 uv = mul(u_model[0], vec4(a_position, 1.0)).xz * u_hScale / u_heightMapSize;

	// determine height and normal
	vec4 sample = texture2DLod(s_normalmapSampler, uv, 1);
	float height = sample.w;

	vec3 normal = sample.xzy;
	normal -= 0.5;
	normal *= 2.0;

	// determine world position
	vec3 position = mul(u_model[0], vec4(a_position.x, height, a_position.z, 1.0)).xyz;

	// generate output
	v_texcoord0 = uv;
	v_position = position;
	v_normal = normal;
	gl_Position = mul(u_viewProj, vec4(position, 1.0));
}
