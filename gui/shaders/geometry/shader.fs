//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal

#include <bgfx.glsl>
#include <light.glsl>
#include <material.glsl>

// uniforms
uniform vec4 u_material[3];
#define u_albedo u_material[0]

#define u_metalic u_material[1].r
#define u_roughness u_material[1].g
#define u_ao u_material[1].b
#define u_hasAlbedoTexture bool(u_material[1].a)

#define u_hasMetalicTexture bool(u_material[2].r)
#define u_hasRoughnessTexture bool(u_material[2].g)
#define u_hasAoTexture bool(u_material[2].b)
#define u_hasNormalTexture bool(u_material[2].a)

// texture samplers
SAMPLER2D(s_albedoTexture, 0);
SAMPLER2D(s_metalicTexture, 1);
SAMPLER2D(s_roughnessTexture, 2);
SAMPLER2D(s_aoTexture, 3);
SAMPLER2D(s_normalsTexture, 4);

// main function
void main() {
	gl_FragColor = u_albedo;
}
