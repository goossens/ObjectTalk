//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec2 vUv;

layout(set=2, binding=0) uniform sampler2D inTexture;

layout(location=0) out vec4 side1Color;
layout(location=1) out vec4 side2Color;
layout(location=2) out vec4 side3Color;
layout(location=3) out vec4 side4Color;
layout(location=4) out vec4 side5Color;
layout(location=5) out vec4 side6Color;

const vec2 invAtan = vec2(0.1591, -0.3183);

vec4 processSide(vec3 v) {
	// convert cartesian direction vector to spherical coordinates
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;

	// sample equirectangular texture
	return texture(inTexture, uv);
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
