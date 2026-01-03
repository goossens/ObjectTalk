//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) out vec3 vPosition;

layout(std140, set=1, binding=0) uniform UBO {
	mat4 invViewProj;
};

void main() {
	vec2 uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	vec4 ndc = vec4(uv * vec2(2.0, -2.0) + vec2(-1.0, 1.0), 1.0, 1.0);

	vec4 pos = invViewProj * ndc;
	vPosition = pos.xyz / pos.w;

	gl_Position = ndc;
}
