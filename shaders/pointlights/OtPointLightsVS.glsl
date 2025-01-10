//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input a_position, i_data0, i_data1
$output v_texcoord0, v_color0

#include <bgfx_shader.glsl>

void main() {
	// decode instance data
	vec3 lightPosition = i_data0.xyz;
	float radius = i_data0.w;
	vec3 color = i_data1.rgb;

	// gather information for fragment shader
	v_texcoord0 = lightPosition;
	v_color0 = vec4(color, radius);
	gl_Position = mul(u_viewProj, vec4(lightPosition + a_position * radius, 1.0));
}
