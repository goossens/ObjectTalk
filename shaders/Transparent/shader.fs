//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx.glsl>
#include <pbr.glsl>

// uniforms
uniform vec4 u_material[5];
#define u_albedo u_material[0]

#define u_metallic u_material[1].r
#define u_roughness u_material[1].g
#define u_ao u_material[1].b

#define u_emissive u_material[2].rgb

#define u_hasAlbedoTexture bool(u_material[3].r)
#define u_hasMetallicRoughnessTexture bool(u_material[3].g)

#define u_hasEmissiveTexture bool(u_material[3].r)
#define u_hasAoTexture bool(u_material[3].g)
#define u_hasNormalTexture bool(u_material[3].b)

uniform vec4 u_lighting[3];
#define u_cameraPosition u_lighting[0].xyz
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz

// texture samplers
SAMPLER2D(s_geometryAlbedoTexture, 0);
SAMPLER2D(s_geometryMetallicRoughnessTexture, 1);
SAMPLER2D(s_geometryEmissiveTexture, 2);
SAMPLER2D(s_geometryAoTexture, 3);
SAMPLER2D(s_geometryNormalTexture, 4);

// main function
void main() {
	// PBR data
	PBR pbr;

	// determine albedo
	if (u_hasAlbedoTexture) {
		pbr.albedo = texture2D(s_geometryAlbedoTexture, v_texcoord0) * u_albedo;

	} else {
		pbr.albedo = u_albedo;
	}

	// discard pixel if too transparent
	if (pbr.albedo.a < 0.3) {
		discard;
	}

	// determine PBR parameters
	pbr.metallic = u_hasMetallicRoughnessTexture ? texture2D(s_geometryMetallicRoughnessTexture, v_texcoord0).b * u_metallic : u_metallic;
	pbr.roughness = u_hasMetallicRoughnessTexture ? texture2D(s_geometryMetallicRoughnessTexture, v_texcoord0).g * u_roughness : u_roughness;
	pbr.emissive = u_hasEmissiveTexture ? texture2D(s_geometryEmissiveTexture, v_texcoord0).rgb * u_emissive : u_emissive;
	pbr.ao = u_hasAoTexture ? texture2D(s_geometryAoTexture, v_texcoord0).r * u_ao : u_ao;

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
