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
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz
#define u_directionalLightAmbience u_lighting[2].a

uniform mat4 u_invViewProjUniform;

// texture samplers
SAMPLER2D(s_deferredLightingAlbedoTexture, 0);
SAMPLER2D(s_deferredLightingNormalTexture, 1);
SAMPLER2D(s_deferredLightingPbrTexture, 2);
SAMPLER2D(s_deferredLightingEmissiveTexture, 3);
SAMPLER2D(s_deferredLightingDepthTexture, 4);

// main function
void main() {
	// ignore pixels without geometry
	vec4 albedoSample = texture2D(s_deferredLightingAlbedoTexture, v_texcoord0);

	if (albedoSample.a == 0.0) {
		discard;
	}

	// determine depth
	float depth = texture2D(s_deferredLightingDepthTexture, v_texcoord0).x;

	// determine world coordinates
	vec4 p = mul(u_invViewProjUniform, vec4(uvToClipSpace(v_texcoord0, depth), 1.0f));
	vec3 pos = p.xyz / p.w;

	// material data
	Material material;
	material.albedo = vec4(albedoSample.rgb, 1.0);
	vec4 data = texture2D(s_deferredLightingPbrTexture, v_texcoord0);
	material.metallic = data.r;
	material.roughness = data.g;
	material.ao = data.b;
	material.N = normalize(texture2D(s_deferredLightingNormalTexture, v_texcoord0).xyz);

	// light data
	DirectionalLight light;
	light.L = normalize(u_directionalLightDirection);
	light.color = u_directionalLightColor;
	light.ambience = u_directionalLightAmbience;

	// apply PBR (tonemapping and gamma correction are done during post-processing)
	vec4 color = directionalLightPBR(material, light, normalize(u_cameraPosition - pos));
	vec3 emissive = texture2D(s_deferredLightingEmissiveTexture, v_texcoord0).rgb;
	gl_FragColor = color + vec4(emissive, 0.0);
	gl_FragDepth = depth;
}
