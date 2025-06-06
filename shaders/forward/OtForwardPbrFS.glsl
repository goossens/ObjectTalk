//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx_shader.glsl>
#include <clip.glsl>
#include <lighting.glsl>
#include <pbr.glsl>
#include <shadow.glsl>

// uniforms
uniform vec4 u_material[5];
#define u_albedo u_material[0]

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

// main function
void main() {
	// apply clip plane
	clipPlane(v_position);

	// determine UV coordinates
	vec2 uv = v_texcoord0 * u_scale + u_offset;

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

	// material data
	Material material;
	material.albedo = toLinear(albedo.rgb);

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
		light.shadow = getShadow(v_position, mul(u_view, vec4(v_position, 1.0)).xyz, dot(material.normal, light.L));

		color += directionalLightPBR(material, light, V);
	}

	// process image based lighting (if required)
	if (u_hasImageBasedLighting) {
		color += imageBasedLightingPBR(material, V, u_iblEnvLevels, s_iblBrdfLut, s_iblIrradianceMap, s_iblEnvironmentMap);
	}

	// finalize color (tonemapping and gamma correction are done during post-processing)
	vec3 emissive = u_hasEmissiveTexture ? texture2D(s_emissiveTexture, uv).rgb * u_emissive : u_emissive;
	gl_FragColor = vec4(color + toLinear(emissive), albedo.a);
}
