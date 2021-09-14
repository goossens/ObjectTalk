//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal

#include <bgfx.glsl>

#include <light.glsl>
#include <material.glsl>

// main function
void main() {
	// get ambient color
	vec4 color = u_light_ambient * u_material_ambient;

	// process all lights
	for (int light = 0; light < LIGHTS; light++) {
		if (u_light_on(light)) {
			color += processLight(light, v_position, v_normal, u_material_diffuse, u_material_specular, u_material_shininess);
		}
	}

	// handle fog
	if (u_fog_enabled) {
		color = processFog(color, v_position);
	}

	// return fragment color
	gl_FragColor = color;
}
