//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <pbr.glsl>
#include <utilities.glsl>

// uniforms
uniform vec4 u_lighting[3];
#define u_cameraPosition u_lighting[0].xyz
#define u_hasDirectionalLighting bool(u_lighting[0].w)
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz
#define u_directionalLightAmbience u_lighting[2].a

uniform mat4 u_invViewProjUniform;

uniform vec4 u_ibl[1];
#define u_hasImageBasedLighting bool(u_ibl[0].x)
#define u_iblEnvLevels int(u_ibl[0].y)

// texture samplers
SAMPLER2D(s_lightingAlbedoTexture, 0);
SAMPLER2D(s_lightingNormalTexture, 1);
SAMPLER2D(s_lightingPbrTexture, 2);
SAMPLER2D(s_lightingEmissiveTexture, 3);
SAMPLER2D(s_lightingDepthTexture, 4);

SAMPLER2D(s_iblBrdfLut, 5);
SAMPLERCUBE(s_iblIrradianceMap, 6);
SAMPLERCUBE(s_iblEnvironmentMap, 7);

// main function
void main() {
	// ignore pixels without geometry
	vec4 albedoSample = texture2D(s_lightingAlbedoTexture, v_texcoord0);

	if (albedoSample.a == 0.0) {
		discard;
	}

	// determine depth
	float depth = texture2D(s_lightingDepthTexture, v_texcoord0).x;

	// determine world coordinates
	vec4 p = mul(u_invViewProjUniform, vec4(uvToClipSpace(v_texcoord0, depth), 1.0f));
	vec3 pos = p.xyz / p.w;

	// determine view direction
	vec3 V = normalize(u_cameraPosition - pos);

	// determine material data
	Material material;
	material.albedo = toLinear(albedoSample.rgb);
	material.normal = texture2D(s_lightingNormalTexture, v_texcoord0).xyz * 2.0 - 1.0;
	vec4 data = texture2D(s_lightingPbrTexture, v_texcoord0);
	material.metallic = data.r;
	material.roughness = data.g;
	material.ao = data.b;

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
	vec3 emissive = texture2D(s_lightingEmissiveTexture, v_texcoord0).rgb;
	gl_FragColor = vec4(color + toLinear(emissive), 1.0);
	gl_FragDepth = depth;
}
