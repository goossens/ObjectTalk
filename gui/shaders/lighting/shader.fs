//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

$input v_texcoord0

#include <bgfx.glsl>

const float PI = 3.14159265359;

// uniforms
uniform mat4 u_inverseTransform;

uniform vec4 u_lighting[3];
#define u_cameraPosition u_lighting[0].xyz
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz

// texture samplers
SAMPLER2D(s_lightingAlbedoTexture, 0);
SAMPLER2D(s_lightingNormalTexture, 1);
SAMPLER2D(s_lightingPbrTexture, 2);
SAMPLER2D(s_lightingDepthTexture, 3);

// determine world coordinates from UV and depth
vec3 uvToWorld(vec2 uv, float depth) {

#if BGFX_SHADER_LANGUAGE_GLSL
	vec3 ndc = vec3(uv * 2.0 - 1.0, depth * 2.0 - 1.0);
#else
	vec3 ndc = vec3(uv * 2.0 - 1.0, depth);
	ndc.y = -ndc.y;
#endif

	vec4 world = mul(u_inverseTransform, vec4(ndc, 1.0));
	return world.xyz / world.w;
}

// Normal Distribution Function (NDF)
float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	float denominator = max(NdotH2 * (a2 - 1.0) + 1.0, 0.0001);
	return a2 / (PI * denominator * denominator);
}

// geometry function
float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

// fresnel equation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// main function
void main() {
	// ignore pixels without geometry
	vec4 albedoSample = texture2D(s_lightingAlbedoTexture, v_texcoord0);

	if (albedoSample.a == 0.0) {
		discard;
	}

	// sample gbuffer
	vec3 albedo = albedoSample.rgb;
	vec3 N = normalize(texture2D(s_lightingNormalTexture, v_texcoord0).rgb);
	vec4 pbr = texture2D(s_lightingPbrTexture, v_texcoord0);
	float metallic = pbr.r;
	float roughness = pbr.g;
	float emissive = pbr.b;
	float ao = pbr.a;

	// determine world coordinates
	vec3 pos = uvToWorld(v_texcoord0, texture2D(s_lightingDepthTexture, v_texcoord0).x);

	// determine view, directional light and halfway vectors
	vec3 V = normalize(u_cameraPosition - pos);
	vec3 L = normalize(u_directionalLightDirection);
	vec3 H = normalize(V + L);

	// calculate reflectance
	vec3 F0 = mix(vec3_splat(0.04), albedo, metallic);

	// Cook-Torrance Bidirectional Reflective Distribution Function (BRDF)
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 specular =
		(NDF * G * F) /
		(4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);

	vec3 kS = F;
	vec3 kD = (vec3_splat(1.0) - kS) * (1.0 - metallic);

	// determine direct light radiance
	float NdotL = max(dot(N, L), 0.0);
	vec3 color = (kD * albedo / PI + specular) * u_directionalLightColor * NdotL;

	// add ambient color
	color += vec3_splat(0.03) * albedo * ao;

	// add emissive light
	color += emissive * albedo;

	// set the HDR color (tonemapping and Gamma correction are done during post-processing)
	gl_FragColor = vec4(color, 1.0);
}
