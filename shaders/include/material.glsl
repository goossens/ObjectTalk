//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef OT_MATERIAL_GLSL
#define OT_MATERIAL_GLSL


// uniforms
uniform vec4 u_material[5];
#define u_albedo u_material[0].rgb

#define u_metallic u_material[1].r
#define u_roughness u_material[1].g
#define u_ao u_material[1].b
#define u_emissive u_material[2].rgb

#define u_offset u_material[3].xy
#define u_scale u_material[3].z

#define u_hasAlbedoTexture bool(u_material[3].a)
#define u_hasMetallicRoughnessTexture bool(u_material[4].r)
#define u_hasEmissiveTexture bool(u_material[4].g)
#define u_hasAoTexture bool(u_material[4].b)
#define u_hasNormalTexture bool(u_material[4].a)

// texture samplers
SAMPLER2D(s_albedoTexture, 0);
SAMPLER2D(s_metallicRoughnessTexture, 1);
SAMPLER2D(s_emissiveTexture, 2);
SAMPLER2D(s_aoTexture, 3);
SAMPLER2D(s_normalTexture, 4);


#endif
