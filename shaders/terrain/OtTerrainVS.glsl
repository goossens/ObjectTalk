//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position

#include <bgfx_shader.glsl>

uniform vec4 u_terrain;
#define u_size u_terrain.x

SAMPLER2D(s_HeightmapTexture, 0);

void main() {
	vec2 uv = mul(u_model[0], vec4(a_position, 1.0)).xz / u_size;
	float height = texture2D(s_HeightmapTexture, uv).r;
	gl_Position = mul(u_modelViewProj, vec4(a_position.x, height, a_position.z, 1.0));
}
