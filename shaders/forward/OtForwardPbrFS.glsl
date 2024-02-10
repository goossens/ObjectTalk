//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx_shader.glsl>
#include <clip.glsl>
#include <pbr.glsl>

// uniforms
uniform vec4 u_pbrMaterial[5];
#define u_albedo u_pbrMaterial[0]

#define u_metallic u_pbrMaterial[1].r
#define u_roughness u_pbrMaterial[1].g
#define u_ao u_pbrMaterial[1].b
#define u_scale u_pbrMaterial[1].a

#define u_emissive u_pbrMaterial[2].rgb

#define u_hasAlbedoTexture bool(u_pbrMaterial[3].r)
#define u_hasMetallicRoughnessTexture bool(u_pbrMaterial[3].g)

#define u_hasEmissiveTexture bool(u_pbrMaterial[4].r)
#define u_hasAoTexture bool(u_pbrMaterial[4].g)
#define u_hasNormalTexture bool(u_pbrMaterial[4].b)

uniform vec4 u_lighting[3];
#define u_cameraPosition u_lighting[0].xyz
#define u_hasDirectionalLighting bool(u_lighting[0].w)
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz
#define u_directionalLightAmbience u_lighting[2].a

uniform vec4 u_ibl[1];
#define u_hasImageBasedLighting bool(u_ibl[0].x)
#define u_iblEnvLevels int(u_ibl[0].y)

// texture samplers
SAMPLER2D(s_albedoTexture, 0);
SAMPLER2D(s_metallicRoughnessTexture, 1);
SAMPLER2D(s_emissiveTexture, 2);
SAMPLER2D(s_aoTexture, 3);
SAMPLER2D(s_normalTexture, 4);

SAMPLER2D(s_iblBrdfLut, 5);
SAMPLERCUBE(s_iblIrradianceMap, 6);
SAMPLERCUBE(s_iblEnvironmentMap, 7);

// main function
void main() {
	// apply clip plane
	clipPlane(v_position);

	// determine UV coordinates
	vec2 uv = v_texcoord0 * u_scale;

	// material data
	Material material;

	// determine albedo
	vec4 albedo;

	if (u_hasAlbedoTexture) {
		albedo = texture2D(s_albedoTexture, uv) * u_albedo;

	} else {
		albedo = u_albedo;
	}

	// discard pixel if too transparent
	if (albedo.a < 0.3) {
		discard;
	}

	material.albedo = albedo.rgb;

	// determine normal
	if (u_hasNormalTexture) {
		vec3 tangent = normalize(v_tangent);
		vec3 bitangent = normalize(v_bitangent);
		vec3 normal = normalize(v_normal);
		mat3 TBN = mtxFromCols(tangent, bitangent, normal);
		material.normal = normalize(mul(TBN, texture2D(s_normalTexture, uv).rgb * 2.0 - 1.0));

	} else {
		material.normal = normalize(v_normal);
	}

	// determine PBR parameters
	material.metallic = u_hasMetallicRoughnessTexture ? texture2D(s_metallicRoughnessTexture, uv).b * u_metallic : u_metallic;
	material.roughness = u_hasMetallicRoughnessTexture ? texture2D(s_metallicRoughnessTexture, uv).g * u_roughness : u_roughness;
	material.ao = u_hasAoTexture ? texture2D(s_aoTexture, uv).r * u_ao : u_ao;

	// determine view direction
	vec3 V = normalize(u_cameraPosition - v_position);

	// total color
	vec3 color = vec3_splat(0.0);

	// process directional light (if required)
	if (u_hasDirectionalLighting) {
		DirectionalLight light;
		light.L = normalize(u_directionalLightDirection);
		light.color = u_directionalLightColor;
		light.ambience = u_directionalLightAmbience;
		color += directionalLightPBR(material, light, V);
	}

	// process image basedlighting (if required)
	if (u_hasImageBasedLighting) {
		color += imageBasedLightingPBR(material, V, u_iblEnvLevels, s_iblBrdfLut, s_iblIrradianceMap, s_iblEnvironmentMap);
	}

	// finalize color (tonemapping and gamma correction are done during post-processing)
	vec3 emissive = texture2D(s_emissiveTexture, v_texcoord0).rgb;
	gl_FragColor = vec4(color + emissive, albedo.a);
}
