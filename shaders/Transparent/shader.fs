//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx.glsl>
#include <pbr.glsl>

// uniforms
uniform vec4 u_material[4];
#define u_albedo u_material[0]

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

uniform vec4 u_lighting[3];
#define u_cameraPosition u_lighting[0].xyz
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz

// texture samplers
SAMPLER2D(s_geometryAlbedoTexture, 0);
SAMPLER2D(s_geometryMetallicTexture, 1);
SAMPLER2D(s_geometryRoughnessTexture, 2);
SAMPLER2D(s_geometryEmissiveTexture, 3);
SAMPLER2D(s_geometryAoTexture, 4);
SAMPLER2D(s_geometryNormalTexture, 5);

// main function
void main() {
	// PBR data
	PBR pbr;

	// determine albedo and convert to gamma space
	if (u_hasAlbedoTexture) {
		vec4 albedoSample = texture2D(s_geometryAlbedoTexture, v_texcoord0);
		pbr.albedo = vec4(pow(albedoSample.rgb, vec3_splat(2.2)), albedoSample.a);

	} else {
		pbr.albedo = vec4(pow(u_albedo.rgb, vec3_splat(2.2)), u_albedo.a);
	}

	if (pbr.albedo.a < 0.3) {
		discard;
	}

	// determine PBR parameters
	pbr.metallic = u_hasMetallicTexture ? texture2D(s_geometryMetallicTexture, v_texcoord0).r : u_metallic;
	pbr.roughness = u_hasRoughnessTexture ? texture2D(s_geometryRoughnessTexture, v_texcoord0).r : u_roughness;
	pbr.emissive = u_hasEmissiveTexture ? texture2D(s_geometryEmissiveTexture, v_texcoord0).r : u_emissive;
	pbr.ao = u_hasAoTexture ? texture2D(s_geometryAoTexture, v_texcoord0).r : u_ao;

	// determine normal
	if (u_hasNormalTexture) {
		vec3 tangent = normalize(v_tangent);
		vec3 bitangent = normalize(v_bitangent);
		vec3 normal = normalize(v_normal);
		mat3 TBN = mtxFromCols(tangent, bitangent, normal);
		pbr.N = normalize(mul(TBN, texture2D(s_geometryNormalTexture, v_texcoord0).rgb * 2.0 - 1.0));

	} else {
		pbr.N = normalize(v_normal);
	}

	// calculate vectors
	pbr.V = normalize(u_cameraPosition - v_position);
	pbr.L = normalize(u_directionalLightDirection);
	pbr.H = normalize(pbr.V + pbr.L);

	// apply PBR (tonemapping and Gamma correction are done during post-processing)
	gl_FragColor = applyPBR(pbr, u_directionalLightColor);
}
