//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#include "bgfx_compute.glsl"

#define THREADS 8

IMAGE2D_WO(outTexture, rgba8, 0);

// uniforms
uniform vec4 u_renderlight[2];
#define u_center u_renderlight[0].xy
#define u_size u_renderlight[0].zw
#define u_color u_renderlight[1].rgb

// functions
NUM_THREADS(THREADS, THREADS, 1)
void main() {
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(outTexture);

	if (coord.x >= size.x || coord.y >= size.y) {
		return;
	}

	vec2 uv = (vec2(coord) + 0.5) / vec2(size);

	float distance = length(abs(uv - u_center) / u_size);
	float brightness = (1.0 - step(1.0, distance)) + (1.0 - smoothstep(1.0, 2.0, distance));
	imageStore(outTexture, coord, vec4(u_color * brightness, 1.0));
}
