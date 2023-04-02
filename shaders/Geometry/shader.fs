//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx.glsl>

// uniforms
uniform vec4 u_material[5];
#define u_albedo u_material[0].rgb

#define u_metallic u_material[1].r
#define u_roughness u_material[1].g
#define u_ao u_material[1].b
#define u_scale u_material[1].a

#define u_emissive u_material[2].rgb

#define u_hasAlbedoTexture bool(u_material[3].r)
#define u_hasMetallicRoughnessTexture bool(u_material[3].g)

#define u_hasEmissiveTexture bool(u_material[4].r)
#define u_hasAoTexture bool(u_material[4].g)
#define u_hasNormalTexture bool(u_material[4].b)

// texture samplers
SAMPLER2D(s_geometryAlbedoTexture, 0);
SAMPLER2D(s_geometryMetallicRoughnessTexture, 1);
SAMPLER2D(s_geometryEmissiveTexture, 2);
SAMPLER2D(s_geometryAoTexture, 3);
SAMPLER2D(s_geometryNormalTexture, 4);

// main function
void main() {
	// determine UV coordinates
	vec2 uv = v_texcoord0 * u_scale;

	// determine albedo
	vec3 albedo = u_albedo;

	if (u_hasAlbedoTexture) {
		albedo = texture2D(s_geometryAlbedoTexture, uv).rgb * albedo;
	}

	// determine normal
	vec3 normal = normalize(v_normal);

	if (u_hasNormalTexture) {
		// apply normal texture
		vec3 tangent = normalize(v_tangent);
		vec3 bitangent = normalize(v_bitangent);
		mat3 TBN = mtxFromCols(tangent, bitangent, normal);
		normal = normalize(mul(TBN, texture2D(s_geometryNormalTexture, uv).rgb * 2.0 - 1.0));
	}

	// determine PBR parameters
	float metallic = u_hasMetallicRoughnessTexture ? texture2D(s_geometryMetallicRoughnessTexture, uv).b * u_metallic: u_metallic;
	float roughness = u_hasMetallicRoughnessTexture ? texture2D(s_geometryMetallicRoughnessTexture, uv).g * u_roughness : u_roughness;
	vec3 emissive = u_hasEmissiveTexture ? texture2D(s_geometryEmissiveTexture, uv).rgb * u_emissive : u_emissive;
	float ao = u_hasAoTexture ? texture2D(s_geometryAoTexture, uv).r * u_ao : u_ao;

	// store information in gbuffer
	gl_FragData[0] = vec4(albedo, 1.0);
	gl_FragData[1] = vec4(normal, 0.0);
	gl_FragData[2] = vec4(metallic, roughness, ao, 0.0);
	gl_FragData[3] = vec4(emissive, 1.0);
}
