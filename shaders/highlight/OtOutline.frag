//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec2 vUv;
layout(location=0) out vec4 fragColor;
layout(set=2, binding=0) uniform sampler2D inTexture;

layout(std140, set=3, binding=0) uniform UBO {
	vec2 texelSize;
};

void main() {
	float value = texture(inTexture, vUv).r;

	if (value > 0.0) {
		discard;
	}

	value += texture(inTexture, vUv + vec2(-texelSize.x, -texelSize.y)).r;
	value += texture(inTexture, vUv + vec2(0.0, -texelSize.y)).r;
	value += texture(inTexture, vUv + vec2(texelSize.x, -texelSize.y)).r;;
	value += texture(inTexture, vUv + vec2(-texelSize.x, 0.0)).r;
	value += texture(inTexture, vUv + vec2(texelSize.x, 0.0)).r;
	value += texture(inTexture, vUv + vec2(-texelSize.x, texelSize.y)).r;
	value += texture(inTexture, vUv + vec2(0.0, texelSize.y)).r;
	value += texture(inTexture, vUv + vec2(texelSize.x, texelSize.y)).r;

	if (value > 0.0) {
		value = 0.5 + value / 9.0;
	}

	fragColor = vec4(0.8, 0.8, 0.0, value);
}
