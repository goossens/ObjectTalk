//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

// light uniforms
#define LIGHTS 4
#define SLOTS_PER_LIGHT 4

uniform vec4 u_light[LIGHTS * SLOTS_PER_LIGHT + 3];
#define u_light_ambient u_light[0]

#define u_fog_enabled int(u_light[1].x)
#define u_fog_near u_light[1].y
#define u_fog_far u_light[1].z
#define u_fog_color u_light[2]

#define u_light_on(l) int(u_light[SLOTS_PER_LIGHT * l + 3].x)
#define u_light_type(l) u_light[SLOTS_PER_LIGHT * l + 3].y
#define u_light_position(l) u_light[SLOTS_PER_LIGHT * l + 4]
#define u_light_diffuse(l) u_light[SLOTS_PER_LIGHT * l + 5]
#define u_light_specular(l) u_light[SLOTS_PER_LIGHT * l + 6]

vec4 processFog(vec4 color, vec3 position) {
	float fog_factor = (u_fog_far - abs(position.z)) / (u_fog_far - u_fog_near);
	return mix(u_fog_color, color, clamp(fog_factor, 0.0, 1.0));
}

// Process a light
vec4 processLight(int light, vec3 position, vec3 normal, vec4 material_diffuse, vec4 material_specular, float material_shininess) {
	// diffuse light
	vec3 light_direction = normalize(u_light_position(light) - position);
	float sDotN = max(dot(light_direction, normal), 0.0);
	vec4 diffuse = u_light_diffuse(light) * material_diffuse * sDotN;

	// specular light
	vec4 specular = vec4(0.0);

	if (sDotN > 0.0) {
		vec3 view_direction = normalize(-position);
		vec3 halfway_direction = normalize(view_direction + light_direction);
		float spec = pow(max(dot(halfway_direction, normal), 0.0), material_shininess);
		specular = u_light_specular(light) * material_specular * spec;
	}

	// calculate fragment color
	return diffuse + specular;
}
