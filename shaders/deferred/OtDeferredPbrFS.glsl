//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx_shader.glsl>
#include <material.glsl>

// main function
void main() {
	// determine UV coordinates
	vec2 uv = v_texcoord0 * u_scale + u_offset;

	// determine albedo
	vec3 albedo = u_albedo;

	if (u_hasAlbedoTexture) {
		albedo = texture2D(s_albedoTexture, uv).rgb * albedo;
	}

	// determine normal
	vec3 normal = normalize(v_normal);

	if (u_hasNormalTexture) {
		// apply normal texture
		vec3 tangent = normalize(v_tangent);
		vec3 bitangent = normalize(v_bitangent);
		mat3 TBN = mtxFromCols(tangent, bitangent, normal);
		normal = normalize(mul(TBN, texture2D(s_normalTexture, uv).rgb * 2.0 - 1.0));
	}

	// determine PBR parameters
	float metallic = u_hasMetallicRoughnessTexture ? texture2D(s_metallicRoughnessTexture, uv).b * u_metallic : u_metallic;
	float roughness = u_hasMetallicRoughnessTexture ? texture2D(s_metallicRoughnessTexture, uv).g * u_roughness : u_roughness;
	vec3 emissive = u_hasEmissiveTexture ? texture2D(s_emissiveTexture, uv).rgb * u_emissive : u_emissive;
	float ao = u_hasAoTexture ? texture2D(s_aoTexture, uv).r * u_ao : u_ao;

	// store information in gbuffer
	gl_FragData[0] = vec4(albedo, 1.0);
	gl_FragData[1] = vec4((normal * 0.5 + 0.5), 1.0);
	gl_FragData[2] = vec4(metallic, roughness, ao, 1.0);
	gl_FragData[3] = vec4(emissive, 1.0);
}
