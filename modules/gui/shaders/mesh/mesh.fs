//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0, v_color0

#include <bgfx.glsl>

#include <fog.glsl>
#include <light.glsl>
#include <material.glsl>

// Process a light
vec4 processLight(int light, vec3 position, vec3 normal, vec4 material_diffuse, vec4 material_specular) {
	// diffuse light
	vec3 light_direction = normalize(u_light_position(light) - position);
	float sDotN = max(dot(light_direction, normal), 0.0);
	vec4 diffuse = u_light_diffuse(light) * material_diffuse * sDotN;

	// specular light
	vec4 specular = vec4(0.0);

	if (sDotN > 0.0) {
		vec3 view_direction = normalize(-position);
		vec3 halfway_direction = normalize(view_direction + light_direction);
		float spec = pow(max(dot(halfway_direction, normal), 0.0), u_material_shininess);
		specular = u_light_specular(light) * material_specular * spec;
	}

	// calculate fragment color
	return diffuse + specular;
}

// main function
void main() {
	vec4 color;

	// raw color don't use material or lighting info
	if (int(u_material_raw)) {
		color = v_color0;

	} else {
		// material colors
		vec4 material_diffuse;
		vec4 material_specular;

		// get ambient color
		if (int(u_material_textured)) {
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
			if (int(u_light_on(light))) {
				color += processLight(light, v_position, v_normal, material_diffuse, material_specular);
			}
		}

		// handle fog
		color = processFog(color, v_position);
	}

	// return fragment color
	gl_FragColor = color;
}
