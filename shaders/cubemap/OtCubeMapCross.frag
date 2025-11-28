//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 vUv;
layout(location=0) out vec4 fragColor;

layout(set=2, binding=0) uniform samplerCube inTexture;

layout(std140, set=3, binding=0) uniform UBO {
	float mipLevel;
};

void main() {
	fragColor = textureLod(inTexture, vUv, mipLevel);
}
