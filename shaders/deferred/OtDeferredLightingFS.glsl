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
	vec3 pos = uvToWorldSpace(v_texcoord0, depth);

	// collect PBR data
	PBR pbr;
	pbr.albedo = vec4(albedoSample.rgb, 1.0);
	vec4 data = texture2D(s_deferredLightingPbrTexture, v_texcoord0);
	pbr.metallic = data.r;
	pbr.roughness = data.g;
	pbr.ao = data.b;
	pbr.emissive = texture2D(s_deferredLightingEmissiveTexture, v_texcoord0).rgb;
	pbr.directionalLightColor = u_directionalLightColor;
	pbr.directionalLightAmbience = u_directionalLightAmbience;

	// calculate vectors
	pbr.N = normalize(texture2D(s_deferredLightingNormalTexture, v_texcoord0).rgb);
	pbr.V = normalize(u_cameraPosition - pos);
	pbr.L = normalize(u_directionalLightDirection);

	// apply PBR (tonemapping and Gamma correction are done during post-processing)
	gl_FragColor = applyPBR(pbr);
	gl_FragDepth = depth;
}
