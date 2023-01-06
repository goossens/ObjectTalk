//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_view, v_normal

#include <bgfx.glsl>
#include <light.glsl>
#include <material.glsl>

// uniforms
uniform vec4 u_material[4];

#define u_color u_material[0]
#define u_ambient u_material[1].rgb
#define u_diffuse u_material[2].rgb
#define u_specular u_material[3].rgb
#define u_shininess u_material[3].a

// main function
void main() {
	Material material = createMaterial(u_ambient, u_diffuse, u_specular, u_shininess);
	gl_FragColor = applyLightAndFog(u_color, material, v_position, v_view, normalize(v_normal));
}
