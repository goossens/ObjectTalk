//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

// fog uniforms
uniform vec4 u_fog[2];
#define u_fog_enabled u_fog[0].x
#define u_fog_near u_fog[0].y
#define u_fog_far u_fog[0].z
#define u_fog_color u_fog[1]

vec4 processFog(vec4 color, vec3 position) {
	if (int(u_fog_enabled)) {
		float fog_factor = (u_fog_far - abs(position.z)) / (u_fog_far - u_fog_near);
		return mix(u_fog_color, color, clamp(fog_factor, 0.0, 1.0));

	} else {
		return color;
	}
}
