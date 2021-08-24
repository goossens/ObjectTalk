//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0, v_color0

#include <bgfx.glsl>

#include <light.glsl>
#include <material.glsl>

// main function
void main() {
	vec4 color;

	// raw color don't use material or light info
	if (u_material_raw) {
		color = v_color0;

	} else {
		// material colors
		vec4 material_diffuse;
		vec4 material_specular;

		// get ambient color
		if (u_material_textured) {
			color = texture2D(s_texture, v_texcoord0);
			material_diffuse = u_material_diffuse * color;
			material_specular = u_material_specular * color;
			color = u_light_ambient * u_material_ambient * color;

		} else {
			color = u_light_ambient * u_material_ambient;
			material_diffuse = u_material_diffuse;
			material_specular = u_material_specular;
		}

		// process all lights
		for (int light = 0; light < LIGHTS; light++) {
			if (u_light_on(light)) {
				color += processLight(light, v_position, v_normal, material_diffuse, material_specular, u_material_shininess);
			}
		}

		// handle fog
		color = processFog(color, v_position);
	}

	// return fragment color
	gl_FragColor = color;
}
