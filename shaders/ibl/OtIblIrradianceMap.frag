//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#include "constants.glsl"

layout(location=0) in vec2 vUv;

layout(set=2, binding=0) uniform samplerCube inTexture;

layout(location=0) out vec4 side1Color;
layout(location=1) out vec4 side2Color;
layout(location=2) out vec4 side3Color;
layout(location=3) out vec4 side4Color;
layout(location=4) out vec4 side5Color;
layout(location=5) out vec4 side6Color;

vec4 processSide(vec3 N) {
	vec3 up = vec3(0.0, 1.0, 0.0);
	const vec3 right = normalize(cross(up, N));
	up = cross(N, right);

	float sampleDelta = 0.1;
	vec3 irradiance = vec3(0.0);
	int sampleCount = 0;

	for (float phi = 0.0; phi < TWO_PI; phi += sampleDelta) {
		for (float theta = 0.0; theta < HALF_PI; theta += sampleDelta) {
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			vec3 sampleVector = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
			irradiance += texture(inTexture, sampleVector).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}

	return vec4(PI * irradiance / float(sampleCount), 1.0);
}

void main() {
	vec2 uv = 2.0 * vec2(vUv.x, 1.0 - vUv.y) - vec2(1.0);

	side1Color = processSide(normalize(vec3(1.0, uv.y, -uv.x)));
	side2Color = processSide(normalize(vec3(-1.0, uv.y, uv.x)));
	side3Color = processSide(normalize(vec3(uv.x, 1.0, -uv.y)));
	side4Color = processSide(normalize(vec3(uv.x, -1.0, uv.y)));
	side5Color = processSide(normalize(vec3(uv.x, uv.y, 1.0)));
	side6Color = processSide(normalize(vec3(-uv.x, uv.y, -1.0)));
}
