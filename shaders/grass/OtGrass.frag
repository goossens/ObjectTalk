//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 vNormal;
layout(location=1) in vec3 vColor;
layout(location=2) in vec4 vData;

layout(location=0) out vec4 fragColor;
layout(location=1) out vec4 fragNormal;
layout(location=2) out vec4 fragPBR;
layout(location=3) out vec4 fragEmissive;

void main() {
	// store information in gbuffer
	fragColor = vec4(mix(vColor, vColor * 0.66, abs(vData.x)), 1.0);
	fragNormal = vec4((normalize(vNormal) * 0.5 + 0.5), 1.0);
	fragPBR = vec4(0.0, 1.0, 1.0, 1.0);
	fragEmissive = vec4(0.0);
}
