//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_LIGHT_GLSL
#define OT_LIGHT_GLSL

#include <material.glsl>
#include <shadow.glsl>

// light uniforms
uniform vec4 u_light[9];
#define u_camera_position u_light[0].xyz

#define u_ambient_color u_light[1].rgb

#define u_light_on bool(u_light[2].x)
#define u_light_position u_light[3].xyz
#define u_light_is_point bool(u_light[3].w)
#define u_light_diffuse u_light[4].rgb
#define u_light_specular u_light[5].rgb

#define u_fog_enabled bool(u_light[6].x)
#define u_fog_near u_light[6].y
#define u_fog_far u_light[6].z
#define u_fog_color u_light[7]

#define u_shadow_enabled bool(u_light[8].r)

// apply lighting to fragment color
vec4 applyLight(vec4 color, vec4 position, vec3 normal, vec4 shadow) {
	// accumulate light types
	vec3 ambient = u_ambient_color * u_material_ambient;
	vec3 diffuse = vec3_splat(0.0);
	vec3 specular = vec3_splat(0.0);

	// process directional light
	if (u_light_on) {
		// normalize the normal
		normal = normalize(normal);

		// determine view and light direction
		vec3 view_direction = normalize(u_camera_position - position.xyz);
		vec3 light_direction;

		if (u_light_is_point) {
			light_direction = normalize(u_light_position - position.xyz);

		} else {
			light_direction = normalize(u_light_position);
		}

		// diffuse light
		float intensity = max(dot(light_direction, normal), 0.0);
		diffuse = u_light_diffuse * u_material_diffuse * intensity;

		if (intensity > 0.0) {
			// specular light
			vec3 halfway_direction = normalize(view_direction + light_direction);
			float spec = pow(max(dot(halfway_direction, normal), 0.0), u_material_shininess);
			specular = u_light_specular * u_material_specular * spec;
		}
	}

	// calculate total light
	vec3 total = ambient;

	// apply shadow (if required)
	if(u_shadow_enabled) {
		vec3 shadowCoords = shadow.xyz / shadow.w;
		shadowCoords = shadowCoords * 0.5 + 0.5;
		float closestDepth = texture2D(s_shadowmap, shadowCoords.xy).r;
		float currentDepth = shadowCoords.z;
		float shade = currentDepth > closestDepth  ? 1.0 : 0.0;
		total += (1.0 - shade) * (diffuse + specular);

	} else {
		total += diffuse + specular;
	}

	// apply light to fragment color
	vec4 result = vec4(total, 1.0) * color;

	// handle fog
	if (u_fog_enabled) {
		float d = distance(u_camera_position, position.xyz);
		float fog_factor = (u_fog_far - d) / (u_fog_far - u_fog_near);
		result = mix(u_fog_color, result, clamp(fog_factor, 0.0, 1.0));
	}

	return result;
}

#endif
