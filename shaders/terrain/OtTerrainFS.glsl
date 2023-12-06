//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0

#include <bgfx_shader.glsl>
#include <OtTerrainUniforms.glsl>

// determine terrain color by region
vec3 getRegionColor(int region, sampler2D s, vec2 uv) {
	return u_regionTextured(region)
		? texture2D(s, uv).rgb
		: u_regionColor(region);
}

// main function
void main() {
	// get parameters
	vec2 uv = v_position.xz / u_textureScale;
	float height = (v_position.y - u_vOffset) / u_vScale;
	float slope = 1.0 - v_normal.y;

	// sample textures
	vec3 region1Albedo = getRegionColor(1, s_region1Sampler, uv);
	vec3 region2Albedo = getRegionColor(2, s_region2Sampler, uv);
	vec3 region3Albedo = getRegionColor(3, s_region3Sampler, uv);
	vec3 region4Albedo = getRegionColor(4, s_region4Sampler, uv);

	// region 1
	vec3 albedo;

	if (height < u_regionTransition1) {
		albedo = region1Albedo;

	// region 1 to region 2 transition
	} else if (height < u_regionTransition1 + u_regionOverlap1) {
		albedo = mix(region1Albedo, region2Albedo, (height - u_regionTransition1) / u_regionOverlap1);

	// region 2
	} else if (height < u_regionTransition2) {
		albedo = region2Albedo;

	// region 2 to region 3 transition
	} else if (height < u_regionTransition2 + u_regionOverlap2) {
		albedo = mix(region2Albedo, region3Albedo, (height - u_regionTransition2) / u_regionOverlap2);

	// region 3
	} else if (height < u_regionTransition3) {
		albedo = region3Albedo;

	// region 3 to region 4 transition
	} else if (height < u_regionTransition3 + u_regionOverlap3) {
		albedo = mix(region3Albedo, region4Albedo, (height - u_regionTransition3) / u_regionOverlap3);

	// region 4
	} else {
		albedo = region4Albedo;
	}

	// store information in gbuffer
	gl_FragData[0] = vec4(albedo, 1.0);
	gl_FragData[1] = vec4(normalize(v_normal), 0.0);
	gl_FragData[2] = vec4(0.0, 1.0, 1.0, 0.0);
	gl_FragData[3] = vec4_splat(0.0);
}
