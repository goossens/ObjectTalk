//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx.glsl>

// uniforms
uniform vec4 u_material[3];
#define u_albedo u_material[0]

#define u_metallic u_material[1].r
#define u_roughness u_material[1].g
#define u_ao u_material[1].b
#define u_hasAlbedoTexture bool(u_material[1].a)

#define u_hasMetallicTexture bool(u_material[2].r)
#define u_hasRoughnessTexture bool(u_material[2].g)
#define u_hasAoTexture bool(u_material[2].b)
#define u_hasNormalTexture bool(u_material[2].a)

// texture samplers
SAMPLER2D(s_geometryAlbedoTexture, 0);
SAMPLER2D(s_geometryMetallicTexture, 1);
SAMPLER2D(s_geometryRoughnessTexture, 2);
SAMPLER2D(s_geometryAoTexture, 3);
SAMPLER2D(s_geometryNormalTexture, 4);

// main function
void main() {
	// determine albedo
	vec4 albedo = u_hasAlbedoTexture ? texture2D(s_geometryAlbedoTexture, v_texcoord0) : u_albedo;

	// discard pixel if too transparent
	if (albedo.w < 0.05) {
		discard;
	}

	// determine normal
	vec3 normal = normalize(v_normal);

	if (u_hasNormalTexture) {
		// apply normal texture
		vec3 tangent = normalize(v_tangent);
		vec3 bitangent = normalize(v_bitangent);
		mat3 TBN = mtxFromCols(tangent, bitangent, normal);
		normal = normalize(mul(TBN, texture2D(s_geometryNormalTexture, v_texcoord0).rgb * 2.0 - 1.0));
	}

	// determine PBR parameters
	float metallic = u_hasMetallicTexture ? texture2D(s_geometryMetallicTexture, v_texcoord0).r : u_metallic;
	float roughness = u_hasRoughnessTexture ? texture2D(s_geometryRoughnessTexture, v_texcoord0).r : u_roughness;
	float ao = u_hasAoTexture ? texture2D(s_geometryAoTexture, v_texcoord0).r : u_ao;

	// store information in gbuffer
	gl_FragData[0] = albedo;
	gl_FragData[1] = vec4(v_position, 0.0);
	gl_FragData[2] = vec4(normal, 0.0);
	gl_FragData[3] = vec4(metallic, roughness, ao, 0.0);
}
