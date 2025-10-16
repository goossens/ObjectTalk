//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Marie, How to make an infinite grid:
//	http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

$input v_near, v_far

#include <bgfx_shader.glsl>

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
	vec4 clipSpacePos = mul(u_viewProj, vec4(worldSpacePos, 1.0));
	vec3 ndcPos = clipSpacePos.xyz / clipSpacePos.w;

	// get the fade factor based on distance in clip space
#if BGFX_SHADER_LANGUAGE_GLSL
	float fading = (ndcPos.z + 1.0) / 2.0;
#else
	float fading = ndcPos.z;
#endif

	// determine grid color
	vec2 coord = worldSpacePos.xz / u_scale;
	vec2 derivative = fwidth(coord);
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
	float lineW = min(grid.x, grid.y);
	float minimumZ = min(derivative.y, 1.0);
	float minimumX = min(derivative.x, 1.0);
	vec4 color = vec4(0.8, 0.8, 0.8, 1.0 - min(lineW, 1.0));
	color.a *= min((1.0 - fading) * 1.2, 1.0);

	gl_FragColor = color;

#if BGFX_SHADER_LANGUAGE_GLSL
	gl_FragDepth = (ndcPos.z + 1.0) * 0.5;
#else
	gl_FragDepth = ndcPos.z;
#endif
}
