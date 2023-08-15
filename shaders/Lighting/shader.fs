//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <pbr.glsl>

// uniforms
uniform mat4 u_inverseTransform;

uniform vec4 u_lighting[3];
#define u_cameraPosition u_lighting[0].xyz
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz
#define u_directionalLightAmbience u_lighting[2].a

// texture samplers
SAMPLER2D(s_lightingAlbedoTexture, 0);
SAMPLER2D(s_lightingNormalTexture, 1);
SAMPLER2D(s_lightingPbrTexture, 2);
SAMPLER2D(s_lightingEmissiveTexture, 3);
SAMPLER2D(s_lightingDepthTexture, 4);

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

// main function
void main() {
	// ignore pixels without geometry
	vec4 albedoSample = texture2D(s_lightingAlbedoTexture, v_texcoord0);

	if (albedoSample.a == 0.0) {
		discard;
	}

	// determine world coordinates
	vec3 pos = uvToWorld(v_texcoord0, texture2D(s_lightingDepthTexture, v_texcoord0).x);

	// collect PBR data
	PBR pbr;
	pbr.albedo = vec4(albedoSample.rgb, 1.0);
	vec4 data = texture2D(s_lightingPbrTexture, v_texcoord0);
	pbr.metallic = data.r;
	pbr.roughness = data.g;
	pbr.ao = data.b;
	pbr.emissive = texture2D(s_lightingEmissiveTexture, v_texcoord0).rgb;
	pbr.directionalLightColor = u_directionalLightColor;
	pbr.directionalLightAmbience = u_directionalLightAmbience;

	// calculate vectors
	pbr.N = normalize(texture2D(s_lightingNormalTexture, v_texcoord0).rgb);
	pbr.V = normalize(u_cameraPosition - pos);
	pbr.L = normalize(u_directionalLightDirection);
	pbr.H = normalize(pbr.V + pbr.L);

	// apply PBR (tonemapping and Gamma correction are done during post-processing)
	gl_FragColor = applyPBR(pbr);
}
