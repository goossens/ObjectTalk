//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$output v_normal, v_color0, v_data0

#include <bgfx_shader.glsl>

#include "common.glsl"

// main function
void main() {
	// determine vertex data
	Vertex vertex = getVertexData(gl_InstanceID, gl_VertexID);

	// get projected vertex position
	gl_Position = mul(u_modelViewProj, vec4(vertex.pos, 1.0));

	// get normal in world space
	v_normal = mul(u_model[0], vec4(vertex.normal, 0.0)).xyz;

	// determine vertex color
	float variance = remap(vertex.hash2.w, -0.5, 0.5, 1.0 - u_colorVariation, 1.0);
	v_color0 = mix(u_baseColor * variance, u_tipColor * variance, vertex.y);

	// pass vertex data
	v_data0 = vec4(vertex.x, 0.0, 0.0, 0.0);
}
