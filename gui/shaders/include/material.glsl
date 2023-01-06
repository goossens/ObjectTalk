#//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_MATERIAL_GLSL
#define OT_MATERIAL_GLSL


//
//	Material structure
//

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};


//
//	Create a material structure
//

Material createMaterial(vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
	Material material;
	material.ambient = ambient;
	material.diffuse = diffuse;
	material.specular = specular;
	material.shininess = shininess;
	return material;
}


#endif
