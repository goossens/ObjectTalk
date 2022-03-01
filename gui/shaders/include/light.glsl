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
uniform vec4 u_light[8];
#define u_camera_position u_light[0].xyz

#define u_ambient_color u_light[1].rgb

#define u_light_enabled bool(u_light[2].x)
#define u_light_position u_light[3].xyz
#define u_light_diffuse u_light[4].rgb
#define u_light_specular u_light[5].rgb

#define u_fog_enabled bool(u_light[6].x)
#define u_fog_near u_light[6].y
#define u_fog_far u_light[6].z
#define u_fog_color u_light[7]


//
//	calculateDiffuse
//

vec3 calculateDiffuse(float intensity) {
	return u_light_diffuse * u_material_diffuse * intensity;
}


//
//	calculateSpecular
//

vec3 calculateSpecular(vec3 view_direction, vec3 light_direction, vec3 normal) {
	// specular light
	vec3 halfway_direction = normalize(view_direction + light_direction);
	float spec = pow(max(dot(halfway_direction, normal), 0.0), u_material_shininess);
	return u_light_specular * u_material_specular * spec;
}


//
//	calculateFog
//

vec4 calculateFog(vec4 position, vec4 color) {
	float d = distance(u_camera_position, position.xyz);
	float fog_factor = (u_fog_far - d) / (u_fog_far - u_fog_near);
	return mix(u_fog_color, color, clamp(fog_factor, 0.0, 1.0));
}


//
//	applyLight
//

vec4 applyLight(vec4 color, vec4 position, vec3 normal, vec4 shadow) {
	// normalize the normal
	normal = normalize(normal);

	// determine view and light direction
	vec3 view_direction = normalize(u_camera_position - position.xyz);

	// get light intensity
	float intensity = max(dot(u_light_position, normal), 0.0);

	// determine light values
	vec3 ambient = u_ambient_color * u_material_ambient;
	vec3 diffuse = u_light_enabled ? calculateDiffuse(intensity) : vec3_splat(0.0);
	vec3 specular = u_light_enabled && intensity > 0.0 ? calculateSpecular(view_direction, u_light_position, normal) : vec3_splat(0.0);

	// apply shadow (if required)
	float shade = u_shadowEnabled ? getShadow(shadow) : 1.0;

	// determine total light factor
	vec3 total = ambient + shade * (diffuse + specular);

	// apply light to fragment color
	vec4 result = vec4(total, 1.0) * color;

	// apply fog if required
	return u_fog_enabled ? calculateFog(position, result) : result;
}

#endif
