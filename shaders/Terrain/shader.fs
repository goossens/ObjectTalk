//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal

#include <bgfx.glsl>

// uniforms
uniform vec4 u_terrain[6];

#define u_regionTransition1 u_terrain[0].x
#define u_regionTransition2 u_terrain[0].y
#define u_regionTransition3 u_terrain[0].z
#define u_scale u_terrain[0].w

#define u_regionOverlap1 u_terrain[1].x
#define u_regionOverlap2 u_terrain[1].y
#define u_regionOverlap3 u_terrain[1].z

#define u_regionColor(i) u_terrain[i + 1].rgb
#define u_regionTextured(i) bool(u_terrain[i + 1].a)

// texture samplers
SAMPLER2D(s_textureSampler0, 0);
SAMPLER2D(s_textureSampler1, 1);
SAMPLER2D(s_textureSampler2, 2);
SAMPLER2D(s_textureSampler3, 3);

// determine terrain color by region
vec3 getRegionColor(int region, sampler2D s, vec2 uv) {
	return u_regionTextured(region)
		? texture2D(s, uv).rgb
		: u_regionColor(region);
}

// main function
void main() {
	vec2 uv = v_position.xz * u_scale;
	float height = v_position.y;
	vec3 albedo;

	// region 1
	if (height < u_regionTransition1) {
		albedo = getRegionColor(1, s_textureSampler0, uv);

	// region 1 to region 2 transition
	} else if (height < u_regionTransition1 + u_regionOverlap1) {
		albedo = mix(
			getRegionColor(1, s_textureSampler0, uv),
			getRegionColor(2, s_textureSampler1, uv),
			(height - u_regionTransition1) / u_regionOverlap1);

	// region 2
	} else if (height < u_regionTransition2) {
		albedo = getRegionColor(2, s_textureSampler1, uv);

	// region 2 to region 3 transition
	} else if (height < u_regionTransition2 + u_regionOverlap2) {
		albedo = mix(
			getRegionColor(2, s_textureSampler1, uv),
			getRegionColor(3, s_textureSampler2, uv),
			(height - u_regionTransition2) / u_regionOverlap2);

	// region 3
	} else if (height < u_regionTransition3) {
		albedo = getRegionColor(3, s_textureSampler2, uv);

	// region 3 to region 4 transition
	} else if (height < u_regionTransition3 + u_regionOverlap3) {
		albedo = mix(
			getRegionColor(3, s_textureSampler2, uv),
			getRegionColor(4, s_textureSampler3, uv),
			(height - u_regionTransition3) / u_regionOverlap3);

	// region 4
	} else {
		albedo = getRegionColor(4, s_textureSampler3, uv);
	}

	// store information in gbuffer
	gl_FragData[0] = vec4(albedo, 1.0);
	gl_FragData[1] = vec4(normalize(v_normal), 0.0);
	gl_FragData[2] = vec4(0.0, 1.0, 1.0, 0.0);
	gl_FragData[3] = vec4_splat(0.0);
}
