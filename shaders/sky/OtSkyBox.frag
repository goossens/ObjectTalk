//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 vPosition;
layout(location=0) out vec4 fragColor;

layout(std140, set=3, binding=0) uniform UBO {
	float brightness;
	float gamma;
};

layout(set=2, binding=0) uniform samplerCube skyTexture;

void main() {
	vec3 color = texture(skyTexture, vPosition).rgb;
	color = pow(color, vec3(gamma));
	color = brightness * color;
	fragColor = vec4(color, 1.0);
}
