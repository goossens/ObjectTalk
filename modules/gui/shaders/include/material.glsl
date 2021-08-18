//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


// material uniforms
uniform vec4 u_material[4];
#define u_material_ambient u_material[0]
#define u_material_diffuse u_material[1]
#define u_material_specular u_material[2]
#define u_material_raw u_material[3].x
#define u_material_textured u_material[3].y
#define u_material_shininess u_material[3].z

uniform mat3 u_uv_transform;

SAMPLER2D(s_texture, 0);
