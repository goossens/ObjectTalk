//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

// light uniforms
#define LIGHTS 3
#define SLOTS_PER_LIGHT 4

uniform vec4 u_light[LIGHTS * SLOTS_PER_LIGHT + 1];
#define u_light_ambient u_light[0]
#define u_light_on(l) u_light[SLOTS_PER_LIGHT * l + 1].x
#define u_light_type(l) u_light[SLOTS_PER_LIGHT * l +1].y
#define u_light_position(l) u_light[SLOTS_PER_LIGHT * l + 2] // in camera space
#define u_light_diffuse(l) u_light[SLOTS_PER_LIGHT * l + 3]
#define u_light_specular(l) u_light[SLOTS_PER_LIGHT * l + 4]
