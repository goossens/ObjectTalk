//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Derived from: https://github.com/patriciogonzalezvivo/lygia

#include "bgfx_compute.glsl"

#define THREADS 8

IMAGE2D_WO(outTexture, rgba8, 0);

uniform vec4 u_checkerboard[3];
#define u_repeat u_checkerboard[0].x
#define u_black_color u_checkerboard[1]
#define u_white_color u_checkerboard[2]

NUM_THREADS(THREADS, THREADS, 1)
void main() {
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(outTexture);

	if (coord.x >= size.x || coord.y >= size.y) {
		return;
	}

	vec2 uv = (vec2(coord) + 0.5) / vec2(size);
	vec2 st = fract(uv * u_repeat) - 0.5;
	imageStore(outTexture, coord, mix(u_black_color, u_white_color, step(st.x * st.y, 0.0)));
}
