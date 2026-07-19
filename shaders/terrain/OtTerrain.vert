//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 aPosition;

layout(location=0) out vec3 vPosition;
layout(location=1) out vec3 vNormal;

// uniforms
layout(std140, set=1, binding=0) uniform TerrainUBO {
	mat4 viewProjectionMatrix;
	float hScale;
	float heightMapDensity;
	float heightMapSize;
};

layout(std140, set=1, binding=1) uniform MeshUBO {
	mat4 modelMatrix;
};

layout(set=0, binding=0) uniform sampler2D normalMapTexture;

void main() {
	// determine height map UV from world space
	vec2 uv = (modelMatrix * vec4(aPosition, 1.0)).xz * hScale * heightMapDensity/ heightMapSize;
	uv.y = 1.0 + uv.y;

	// determine height and normal (in world space)
	vec4 normalMapSample = textureLod(normalMapTexture, uv, 0.0);
	float height = normalMapSample.w;
	vec3 normal = normalMapSample.xyz;
	normal *= vec3(1.0, 1.0, -1.0);

	// determine real world position (including height)
	vec3 position = (modelMatrix * vec4(aPosition.x, height, aPosition.z, 1.0)).xyz;

	// generate output
	vPosition = position;
	vNormal = normal;
	gl_Position = viewProjectionMatrix * vec4(position, 1.0);
}
