//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$output v_texcoord0

#include <bgfx_shader.glsl>

#include "common.glsl"

// main function
void main() {
	// determine vertex data
	Vertex vertex = getVertexData(gl_InstanceID, gl_VertexID);

	// get projected vertex position
	gl_Position = mul(u_modelViewProj, vec4(vertex.pos, 1.0));
}
