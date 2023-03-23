//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include <bgfx.glsl>

// uniforms
uniform vec4 u_blendmap[2];

#define u_metallic u_blendmap[0].r
#define u_roughness u_blendmap[0].g
#define u_scale u_blendmap[0].b

#define u_hasNoneNormals bool(u_blendmap[1].r)
#define u_hasRedNormals bool(u_blendmap[1].g)
#define u_hasGreenNormals bool(u_blendmap[1].b)
#define u_hasBlueNormals bool(u_blendmap[1].a)

// texture samplers
SAMPLER2D(s_BlendMap, 0);

SAMPLER2D(a_AlbedoNone, 1);
SAMPLER2D(a_AlbedoRed, 2);
SAMPLER2D(a_AlbedoGreen, 3);
SAMPLER2D(a_AlbedoBlue, 4);

SAMPLER2D(s_NormalsNone, 5);
SAMPLER2D(s_NormalsRed, 6);
SAMPLER2D(s_NormalsGreen, 7);
SAMPLER2D(s_NormalsBlue, 8);

// main function
void main() {
	// blend texture colors based on blendmap
	vec4 blend = texture2D(s_BlendMap, v_texcoord0);
	float n = 1.0 - blend.r - blend.g - blend.b;
	vec2 uv = v_texcoord0 * u_scale;

	vec3 albedo = (
		texture2D(a_AlbedoNone, uv) * n +
		texture2D(a_AlbedoRed, uv) * blend.r +
		texture2D(a_AlbedoGreen, uv) * blend.g +
		texture2D(a_AlbedoBlue, uv) * blend.b).rgb;

	// determine TBN matrix
	vec3 tangent = normalize(v_tangent);
	vec3 bitangent = normalize(v_bitangent);
	vec3 normal = normalize(v_normal);
	mat3 TBN = mtxFromCols(tangent, bitangent, normal);

	// determine normal
	if (u_hasNoneNormals || u_hasRedNormals || u_hasGreenNormals || u_hasBlueNormals) {
		vec3 noneNormal = u_hasNoneNormals ? normalize(mul(TBN, texture2D(s_NormalsNone, uv).rgb * 2.0 - 1.0)) : normal;
		vec3 redNormal = u_hasRedNormals ? normalize(mul(TBN, texture2D(s_NormalsRed, uv).rgb * 2.0 - 1.0)) : normal;
		vec3 greenNormal = u_hasGreenNormals ? normalize(mul(TBN, texture2D(s_NormalsGreen, uv).rgb * 2.0 - 1.0)) : normal;
		vec3 blueNormal = u_hasBlueNormals ? normalize(mul(TBN, texture2D(s_NormalsBlue, uv).rgb * 2.0 - 1.0)) : normal;

		normal = normalize(noneNormal * n + redNormal * blend.r + greenNormal * blend.g + blueNormal * blend.b);
	}

	// store information in gbuffer
	gl_FragData[0] = vec4(albedo, 1.0);
	gl_FragData[1] = vec4(normal, 0.0);
	gl_FragData[2] = vec4(u_metallic, u_roughness, 1.0, 0.0);
	gl_FragData[3] = vec4_splat(0.0);
}
