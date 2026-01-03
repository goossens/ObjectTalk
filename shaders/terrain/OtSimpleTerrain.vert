//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 aPosition;
layout(location=0) out vec2 vUv; // just a dummy to match the fragment shaders

// uniforms
layout(std140, set=1, binding=0) uniform TerrainUBO {
	mat4 viewProjectionMatrix;
	float hScale;
	float heightMapSize;
};

layout(std140, set=1, binding=1) uniform MeshUBO {
	mat4 modelMatrix;
};

layout(set=0, binding=0) uniform sampler2D normalMapTexture;

// functions
void main() {
	// determine height map coordinates
	vec2 uv = (modelMatrix * vec4(aPosition, 1.0)).xz * hScale / heightMapSize;

	// determine height
	float height = texture(normalMapTexture, uv).w;

	// determine world position
	vec3 position = (modelMatrix * vec4(aPosition.x, height, aPosition.z, 1.0)).xyz;

	// generate output
	gl_Position = viewProjectionMatrix * vec4(position, 1.0);
}
