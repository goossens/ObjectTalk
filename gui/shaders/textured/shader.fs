//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_texcoord0, v_shadow

#include <bgfx.glsl>
#include <light.glsl>
#include <material.glsl>

// uniforms
uniform vec4 u_material[3];

#define u_ambient u_material[0].rgb
#define u_scale u_material[0].a
#define u_diffuse u_material[1].rgb
#define u_specular u_material[2].rgb
#define u_shininess u_material[2].a

// samplers
SAMPLER2D(s_material, 1);

// main function
void main() {
	// get texture color
	vec4 color = texture2D(s_material, v_texcoord0 / u_scale);

	// discard pixel if too transparent
	if (color.w < 0.05) {
		discard;
	}

	Material material = createMaterial(u_ambient, u_diffuse, u_specular, u_shininess);
	gl_FragColor = applyLightAndFog(color, material, v_position, normalize(v_normal), v_shadow);
}
