//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Marie, How to make an infinite grid:
//	http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

#version 450 core

layout(location=0) in vec3 vNear;
layout(location=1) in vec3 vFar;

layout(location=0) out vec4 fragColor;

layout(std140, set=3, binding=0) uniform UBO {
	mat4 viewProjectionMatrix;
	float scale;
};

void main() {
	// clip to visible plane
	float t = -vNear.y / (vFar.y - vNear.y);

	if (t < 0.0) {
		discard;
	}

	// get the world and clip space positions
	vec3 worldSpacePos = vNear + t * (vFar - vNear);
	vec4 clipSpacePos = viewProjectionMatrix * vec4(worldSpacePos, 1.0);
	vec3 ndcPos = clipSpacePos.xyz / clipSpacePos.w;

	// get the fade factor based on distance in clip space
	float fading = ndcPos.z;

	// determine grid color
	vec2 coord = worldSpacePos.xz / scale;
	vec2 derivative = fwidth(coord);
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
	float lineW = min(grid.x, grid.y);
	float minimumZ = min(derivative.y, 1.0);
	float minimumX = min(derivative.x, 1.0);
	vec4 color = vec4(0.8, 0.8, 0.8, 1.0 - min(lineW, 1.0));
	color.a *= min((1.0 - fading) * 1.2, 1.0);

	// save values
	fragColor = color;
	gl_FragDepth = ndcPos.z;
}
