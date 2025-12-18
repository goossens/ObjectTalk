//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec2 vUv;

layout(set=2, binding=0) uniform sampler2D inTexture;
layout(location=0) out vec4 fragColor;

layout(std140, set=3, binding=0) uniform UBO {
	int side;
};

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

	if (side == 0) {
		fragColor = processSide(normalize(vec3(1.0, uv.y, -uv.x)));

	} else if (side == 1) {
		fragColor = processSide(normalize(vec3(-1.0, uv.y, uv.x)));

	} else if (side == 2) {
		fragColor = processSide(normalize(vec3(uv.x, 1.0, -uv.y)));

	} else if (side == 3) {
		fragColor = processSide(normalize(vec3(uv.x, -1.0, uv.y)));

	} else if (side == 4) {
		fragColor = processSide(normalize(vec3(uv.x, uv.y, 1.0)));

	} else {
		fragColor = processSide(normalize(vec3(-uv.x, uv.y, -1.0)));
	}
}
