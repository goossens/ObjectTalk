//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx.glsl>

// uniforms
uniform vec4 u_material[4];
#define u_albedo u_material[0].rgb

#define u_metallic u_material[1].r
#define u_roughness u_material[1].g
#define u_emissive u_material[1].b
#define u_ao u_material[1].a

#define u_hasAlbedoTexture bool(u_material[2].r)
#define u_hasMetallicTexture bool(u_material[2].g)
#define u_hasRoughnessTexture bool(u_material[2].b)

#define u_hasEmissiveTexture bool(u_material[3].r)
#define u_hasAoTexture bool(u_material[3].g)
#define u_hasNormalTexture bool(u_material[3].b)

// texture samplers
SAMPLER2D(s_geometryAlbedoTexture, 0);
SAMPLER2D(s_geometryMetallicTexture, 1);
SAMPLER2D(s_geometryRoughnessTexture, 2);
SAMPLER2D(s_geometryEmissiveTexture, 3);
SAMPLER2D(s_geometryAoTexture, 4);
SAMPLER2D(s_geometryNormalTexture, 5);

// main function
void main() {
	// determine albedo
	vec3 albedo = u_albedo;

	if (u_hasAlbedoTexture) {
		// use texture and convert to gamma space
		vec4 albedoSample = texture2D(s_geometryAlbedoTexture, v_texcoord0);

		// discard pixel if it represents a hole
		if (albedoSample.a < 0.5) {
			discard;
		}

		albedo = pow(albedoSample.rgb, vec3_splat(2.2));
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
	float emissive = u_hasEmissiveTexture ? texture2D(s_geometryEmissiveTexture, v_texcoord0).r : u_emissive;
	float ao = u_hasAoTexture ? texture2D(s_geometryAoTexture, v_texcoord0).r : u_ao;

	// store information in gbuffer
	gl_FragData[0] = vec4(albedo, 1.0);
	gl_FragData[1] = vec4(normal, 0.0);
	gl_FragData[2] = vec4(metallic, roughness, emissive, ao);
}
