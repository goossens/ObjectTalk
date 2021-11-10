//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position_world, v_position_camera, v_normal_world, v_normal

#include <bgfx.glsl>
#include <light.glsl>

uniform vec4 u_land[9];

#define textureScale u_land[0].x

#define regionMinHeight(i) u_land[(i - 1) * 2 + 1].x
#define regionMaxHeight(i) u_land[(i - 1) * 2 + 1].y
#define regionTextured(i) bool(u_land[(i - 1) * 2 + 1].z)
#define regionColor(i) u_land[(i - 1) * 2 + 2].rgb

SAMPLER2D(s_texture_region_1, 0);
SAMPLER2D(s_texture_region_2, 1);
SAMPLER2D(s_texture_region_3, 2);
SAMPLER2D(s_texture_region_4, 3);

// determine land color by region
vec3 getRegionColor(int region, sampler2D s, vec3 position, vec3 normal) {
	vec3 color;

	if (regionTextured(region)) {
		vec3 blending = abs(normal);
		blending = normalize(max(blending, 0.00001));
		blending /= vec3(blending.x + blending.y + blending.z);

		vec3 xColor = texture2D(s, position.yz * textureScale).rgb * blending.x;
		vec3 yColor = texture2D(s, position.xz * textureScale).rgb * blending.y;
		vec3 zColor = texture2D(s, position.xy * textureScale).rgb * blending.z;

		color = xColor + yColor + zColor, 1.0;

	} else {
		color = regionColor(region);
	}

	float range = regionMaxHeight(region) - regionMinHeight(region);
	float weight = max(0.0, (range - abs(position.y - regionMaxHeight(region))) / range);
	return weight * color;
}

// main function
void main() {
	// determine color based on land regions
	vec3 landColor = getRegionColor(1, s_texture_region_1, v_position_world, v_normal_world);
	landColor += getRegionColor(2, s_texture_region_2, v_position_world, v_normal_world);
	landColor += getRegionColor(3, s_texture_region_3, v_position_world, v_normal_world);
	landColor += getRegionColor(4, s_texture_region_4, v_position_world, v_normal_world);
	gl_FragColor = applyLight(vec4(landColor, 1.0), v_position_camera, v_normal);
}
