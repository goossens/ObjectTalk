//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_view, v_normal, v_texcoord0

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

SAMPLER2D(s_material1, 1);
SAMPLER2D(s_material2, 2);
SAMPLER2D(s_material3, 3);
SAMPLER2D(s_material4, 4);
SAMPLER2D(s_material5, 5);

// main function
void main() {
	// blend texture colors based on blendmap
	vec4 blend = texture2D(s_material1, v_texcoord0);
	float b = 1.0 - blend.r - blend.g - blend.b;
	vec2 tiled = v_texcoord0 / u_scale;

	vec4 color = texture2D(s_material2, tiled) * b +
		texture2D(s_material3, tiled) * blend.r +
		texture2D(s_material4, tiled) * blend.g +
		texture2D(s_material5, tiled) * blend.b;

	Material material = createMaterial(u_ambient, u_diffuse, u_specular, u_shininess);
	gl_FragColor = applyLightAndFog(color, material, v_position, v_view, normalize(v_normal));
}
