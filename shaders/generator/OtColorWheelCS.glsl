//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#include "bgfx_compute.glsl"
#include "constants.glsl"

#define THREADS 8

IMAGE2D_WO(outTexture, rgba8, 0);

vec3 colorPalette(float distance, float angle) {
	float theta = mod(3.0 + 3.0 * angle / PI + 1.5, 6.0);
	vec3 result = clamp(abs(mod(theta + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
	result = mix(vec3_splat(1.0), result, distance);
	return result;
}

float stableAtan2(float y, float x) {
	return mix(PI / 2.0 - atan2(x, y), atan2(y, x), abs(x) > abs(y));
}

NUM_THREADS(THREADS, THREADS, 1)
void main() {
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(outTexture);

	if (coord.x >= size.x || coord.y >= size.y) {
		return;
	}

	vec2 sizef = size;
	vec2 uv = vec2(coord) / sizef;

	vec2 scale = (sizef.x > sizef.y) ? vec2(1.0, sizef.x / sizef.y) : vec2(sizef.y / sizef.x, 1.0f);
	vec2 xy = (uv - 0.5) * 2.0 * scale;
	float distance = length(xy);
	float angle = stableAtan2(xy.x, xy.y);

	vec3 color = (distance > 1.0) ? vec3_splat(0.0) : colorPalette(distance, angle);
	imageStore(outTexture, coord, vec4(color, 1.0));
}
