//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 vData0;
layout(location=1) in vec3 vData1;

layout(location=0) out vec4 fragColor;

layout(set=2, binding=0) uniform sampler2D particlesTexture;

void main() {
	vec2 uv1 = vData0.xy;
	vec2 uv2 = vData1.xy;
	float alpha = vData0.z;
	float blend = vData1.z;

	vec4 color1 = texture(particlesTexture, uv1);
	vec4 color2 = texture(particlesTexture, uv2);
	vec4 color = mix(color1, color2, blend);
	color.a *= alpha;

	if (color.a < 0.1) {
		discard;
	}

	fragColor = vec4(color);
}
