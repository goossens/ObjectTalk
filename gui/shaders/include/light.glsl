//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_LIGHT_GLSL
#define OT_LIGHT_GLSL

#include <material.glsl>

// light uniforms
#define LIGHTS 4
#define SLOTS_PER_LIGHT 4

uniform vec4 u_light[LIGHTS * SLOTS_PER_LIGHT + 3];
#define u_ambient_color u_light[0].rgb

#define u_fog_enabled bool(u_light[1].x)
#define u_fog_near u_light[1].y
#define u_fog_far u_light[1].z
#define u_fog_color u_light[2]

#define u_light_on(l) bool(u_light[SLOTS_PER_LIGHT * l + 3].x)
#define u_light_position(l) u_light[SLOTS_PER_LIGHT * l + 4]
#define u_light_diffuse(l) u_light[SLOTS_PER_LIGHT * l + 5].rgb
#define u_light_specular(l) u_light[SLOTS_PER_LIGHT * l + 6].rgb

// apply lighting to fragment color
vec4 applyLight(vec4 color, vec3 position, vec3 normal) {
	// accumulate light types
	vec3 ambient = u_ambient_color * u_material_ambient;
	vec3 diffuse = vec3_splat(0.0);
	vec3 specular = vec3_splat(0.0);

	// calculate view direction only once
	vec3 view_direction = normalize(-position);

	// process all point lights
	for (int light = 0; light < LIGHTS; light++) {
		if (u_light_on(light)) {
			// determine light direction
			vec3 light_direction;

			if (u_light_position(light).w == 0.0) {
				// this is a directional light
				light_direction = normalize(u_light_position(light).xyz);

			} else {
				// this is a point light
				light_direction = normalize(u_light_position(light).xyz - position);
			}

			// diffuse light
			float intensity = max(dot(light_direction, normal), 0.0);
			diffuse += u_light_diffuse(light) * u_material_diffuse * intensity;

			if (intensity > 0.0) {
				// specular light
				vec3 halfway_direction = normalize(view_direction + light_direction);
				float spec = pow(max(dot(halfway_direction, normal), 0.0), u_material_shininess);
				specular += u_light_specular(light) * u_material_specular * spec;
			}
		}
	}

	// calculate total light
	vec3 total = ambient + diffuse + specular;

	// apply light to fragment color
	vec4 result = vec4(total, 1.0) * color;

	// handle fog
	if (u_fog_enabled) {
		float fog_factor = (u_fog_far - abs(position.z)) / (u_fog_far - u_fog_near);
		result = mix(u_fog_color, result, clamp(fog_factor, 0.0, 1.0));
	}

	return result;
}

#endif