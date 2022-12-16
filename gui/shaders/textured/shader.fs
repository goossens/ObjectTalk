//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_texcoord0, v_shadow

#include <bgfx.glsl>
#include <light.glsl>
#include <material.glsl>

// uniforms
uniform vec4 u_material[3];

#define u_ambient u_material[0].rgb
#define u_scale u_material[0].a
#define u_diffuse u_material[1].rgb
#define u_normalled bool(u_material[1].a)
#define u_specular u_material[2].rgb
#define u_shininess u_material[2].a

// samplers
SAMPLER2D(s_texture, 1);
SAMPLER2D(s_normalmap, 2);

// main function
void main() {
	// get texture color
	vec4 color = texture2D(s_texture, v_texcoord0 / u_scale);

	// discard pixel if too transparent
	if (color.w < 0.05) {
		discard;
	}

	// determine normal
	vec3 normal = normalize(v_normal);

	if (u_normalled) {
		// apply normalmap
		vec3 tangent = normalize(v_tangent);
		tangent = normalize(tangent - dot(tangent, normal) * normal);
		vec3 bitangent = cross(tangent, normal);
		vec3 bumpMapNormal = texture2D(s_normalmap, v_texcoord0 / u_scale).xyz;
		bumpMapNormal = 2.0 * bumpMapNormal - vec3_splat(1.0);
		mat3 TBN = mtxFromCols(tangent, bitangent, normal);
		normal = normalize(mul(TBN, bumpMapNormal));
	}

	Material material = createMaterial(u_ambient, u_diffuse, u_specular, u_shininess);
	gl_FragColor = applyLightAndFog(color, material, v_position, normal, v_shadow);
}
