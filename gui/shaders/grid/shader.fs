//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Marie, How to make an infinite grid:
//	http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

$input v_near, v_far

#include <bgfx.glsl>

uniform vec4 u_grid[1];
#define u_scale u_grid[0].x

// main program
void main() {
	// clip to visible plane
	float t = -v_near.y / (v_far.y - v_near.y);

	if (t < 0.0) {
		discard;
	}

	// get the world and clip space positions
	vec3 worldSpacePos = v_near + t * (v_far - v_near);
	vec4 pos = mul(u_viewProj, vec4(worldSpacePos, 1.0));
	vec3 clipSpacePos = pos.xyz / pos.w;

	// get the fade factor based on distance in clip space
#if BGFX_SHADER_LANGUAGE_GLSL
	float fading = (clipSpacePos.z + 1.0) / 2.0;
#else
	float fading = clipSpacePos.z;
#endif

	// determine grid color
	vec2 coord = worldSpacePos.xz / u_scale;
	vec2 derivative = fwidth(coord);
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
	float lineW = min(grid.x, grid.y);
	float minimumZ = min(derivative.y, 1.0);
	float minimumX = min(derivative.x, 1.0);
	vec4 color = vec4(0.3, 0.3, 0.3, 1.0 - min(lineW, 1.0));

	if (worldSpacePos.x > -0.1 * minimumX && worldSpacePos.x < 0.1 * minimumZ) {
		color.z = 1.0;
	}

	if (worldSpacePos.z > -0.1 * minimumX && worldSpacePos.z < 0.1 * minimumZ) {
		color.x = 1.0;
	}

	color.a *= min((1.0 - fading) * 1.2, 1.0);

	gl_FragColor = color;
	gl_FragDepth = clipSpacePos.z;
}
