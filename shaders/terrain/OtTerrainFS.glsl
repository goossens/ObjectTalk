//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0

#include <bgfx_shader.glsl>
#include <clip.glsl>

#include <OtTerrainUniforms.glsl>

// determine terrain color by region
vec3 getRegionColor(int region, sampler2D s, vec2 uv) {
	return u_regionTextured(region)
		? texture2D(s, uv).rgb
		: u_regionColor(region);
}

// main function
void main() {
	// apply clip plane
	clipPlane(v_position);

	// get parameters
	vec2 uv1 = fract(v_position.xz / u_region1TextureSize * u_region1TextureScale);
	vec2 uv2 = fract(v_position.xz / u_region2TextureSize * u_region2TextureScale);
	vec2 uv3 = fract(v_position.xz / u_region3TextureSize * u_region3TextureScale);
	vec2 uv4 = fract(v_position.xz / u_region4TextureSize * u_region4TextureScale);
	float height = (v_position.y - u_vOffset) / u_vScale;

	// sample textures
	vec3 region1Albedo = getRegionColor(1, s_region1Texture, uv1);
	vec3 region2Albedo = getRegionColor(2, s_region2Texture, uv2);
	vec3 region3Albedo = getRegionColor(3, s_region3Texture, uv3);
	vec3 region4Albedo = getRegionColor(4, s_region4Texture, uv4);

	// adjust texture by slope
	float slope = abs(dot(v_normal, vec3(0.0, 1.0, 0.0)));
	float coef = 1.0 - smoothstep(0.5, 0.6, slope);
	region2Albedo = mix(region2Albedo, region3Albedo, coef);
	region4Albedo = mix(region4Albedo, region3Albedo, coef);

	// region 1
	vec3 albedo;

	if (height < u_region1Transition) {
		albedo = region1Albedo;

	// region 1 to region 2 transition
	} else if (height < u_region1Transition + u_region1Overlap) {
		albedo = mix(region1Albedo, region2Albedo, (height - u_region1Transition) / u_region1Overlap);

	// region 2
	} else if (height < u_region2Transition) {
		albedo = region2Albedo;

	// region 2 to region 3 transition
	} else if (height < u_region2Transition + u_region2Overlap) {
		albedo = mix(region2Albedo, region3Albedo, (height - u_region2Transition) / u_region2Overlap);

	// region 3
	} else if (height < u_region3Transition) {
		albedo = region3Albedo;

	// region 3 to region 4 transition
	} else if (height < u_region3Transition + u_region3Overlap) {
		albedo = mix(region3Albedo, region4Albedo, (height - u_region3Transition) / u_region3Overlap);

	// region 4
	} else {
		albedo = region4Albedo;
	}

	// store information in gbuffer
	gl_FragData[0] = vec4(albedo, 1.0);
	gl_FragData[1] = vec4((normalize(v_normal) * 0.5 + 0.5), 1.0);
	gl_FragData[2] = vec4(0.0, 1.0, 1.0, 1.0);
	gl_FragData[3] = vec4_splat(0.0);
}
