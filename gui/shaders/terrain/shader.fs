//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_view, v_normal

#include <bgfx.glsl>
#include <light.glsl>
#include <material.glsl>

uniform vec4 u_terrain[7];

#define textureScale u_terrain[0].x
#define renderReflection bool(u_terrain[0].y)
#define renderRefraction bool(u_terrain[0].z)

#define regionTransition1 u_terrain[1].x
#define regionTransition2 u_terrain[1].y
#define regionTransition3 u_terrain[1].z

#define regionOverlap1 u_terrain[2].x
#define regionOverlap2 u_terrain[2].y
#define regionOverlap3 u_terrain[2].z

#define regionColor(i) u_terrain[i + 2].rgb
#define regionTextured(i) bool(u_terrain[i + 2].w)

SAMPLER2D(s_texture_region_1, 1);
SAMPLER2D(s_texture_region_2, 2);
SAMPLER2D(s_texture_region_3, 3);
SAMPLER2D(s_texture_region_4, 4);

// determine terrain color by region
vec3 getSimpleRegionColor(int region, sampler2D s, vec3 position) {
	return regionTextured(region)
		? texture2D(s, position.xz * textureScale).rgb
		: regionColor(region);
}

vec3 getRegionColor(int region, sampler2D s, vec3 position, vec3 normal) {
	normal = normalize(normal);

	if (regionTextured(region)) {
		vec3 blending = abs(normal);
		blending = normalize(max(blending, 0.00001));
		blending /= vec3_splat(blending.x + blending.y + blending.z);

		vec3 xColor = texture2D(s, position.yz / textureScale).rgb * blending.x;
		vec3 yColor = texture2D(s, position.xz / textureScale).rgb * blending.y;
		vec3 zColor = texture2D(s, position.xy / textureScale).rgb * blending.z;
		return xColor + yColor + zColor;

	} else {
		return regionColor(region);
	}
}

// main function
void main() {
		if (renderReflection && v_position.y <= 0.0) {
		discard;
	}

	if (renderRefraction && v_position.y >= 0.0) {
		discard;
	}

	// determine color based on regions
	float height = v_position.y;
	float slope = (1.0 - normalize(v_normal).y) * 1.7;
	vec3 terrainColor;

	// sand region
	if (height < regionTransition1) {
		terrainColor = getSimpleRegionColor(1, s_texture_region_1, v_position);

	// sand to grass region
	} else if (height < regionTransition1 + regionOverlap1) {
		terrainColor = mix(
			getSimpleRegionColor(1, s_texture_region_1, v_position),
			getSimpleRegionColor(2, s_texture_region_2, v_position),
			(height - regionTransition1) / regionOverlap1);

	// grass region
	} else if (height < regionTransition2) {
		terrainColor = getSimpleRegionColor(2, s_texture_region_2, v_position);

	// grass to grass/rock region
	} else if (height < regionTransition2 + regionOverlap2) {
		vec3 grassRock = mix(
			getRegionColor(2, s_texture_region_2, v_position, v_normal),
			getRegionColor(3, s_texture_region_3, v_position, v_normal),
			slope);

		terrainColor = mix(
			getSimpleRegionColor(2, s_texture_region_2, v_position),
			grassRock,
			(height - regionTransition2) / regionOverlap2);

	// rock/grass region
	} else if (height < regionTransition3) {
		terrainColor = mix(
			getRegionColor(2, s_texture_region_2, v_position, v_normal),
			getRegionColor(3, s_texture_region_3, v_position, v_normal),
			slope);

	// rock/grass to snow region
	} else if (height < regionTransition3 + regionOverlap3) {
		vec3 grassRock = mix(
			getRegionColor(2, s_texture_region_2, v_position, v_normal),
			getRegionColor(3, s_texture_region_3, v_position, v_normal),
			slope);

		terrainColor = mix(
			grassRock,
			getSimpleRegionColor(4, s_texture_region_4, v_position),
			(height - regionTransition3) / regionOverlap3);

	// we must be in the snow/rock region
	} else {
		terrainColor = getSimpleRegionColor(4, s_texture_region_4, v_position);
	}

	// apply lighting
	Material material = createMaterial(vec3_splat(0.6), vec3_splat(0.6), vec3_splat(0.5), 20.0);
	gl_FragColor = applyLightAndFog(vec4(terrainColor, 1.0), material, v_position, v_view, normalize(v_normal));

}
