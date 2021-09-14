//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0

#include <bgfx.glsl>

#include <light.glsl>
#include <material.glsl>

SAMPLER2D(s_texture, 0);

// main function
void main() {
	// get texture color
	vec4 color = texture2D(s_texture, v_texcoord0);

	// material colors
	vec4 material_diffuse = u_material_diffuse * color;
	vec4 material_specular = u_material_specular * color;

	// ambient light
	color = u_light_ambient * u_material_ambient * color;

	// process all lights
	for (int light = 0; light < LIGHTS; light++) {
		if (u_light_on(light)) {
			color += processLight(light, v_position, v_normal, material_diffuse, material_specular, u_material_shininess);
		}
	}

	// handle fog
	if (u_fog_enabled) {
		color = processFog(color, v_position);
	}

	// return fragment color
	gl_FragColor = color;
}
