//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#define LIGHTING_UNIFORMS 1
#define LIGHTING_SAMPLERS 5
#include "lighting.glsl"

#define SHADOW_UNIFORMS 2
#define SHADOW_SAMPLERS 8
#include "shadow.glsl"

#include "pbr.glsl"
#include "utilities.glsl"

layout(location=0) in vec2 vUv;
layout(location=0) out vec4 fragColor;

// uniforms
layout(std140, set=3, binding=0) uniform UBO {
	mat4 viewUniform;
	mat4 invViewProjUniform;
};

// texture samplers
layout(set=2, binding=0) uniform sampler2D lightingAlbedoTexture;
layout(set=2, binding=1) uniform sampler2D lightingNormalTexture;
layout(set=2, binding=2) uniform sampler2D lightingPbrTexture;
layout(set=2, binding=3) uniform sampler2D lightingEmissiveTexture;
layout(set=2, binding=4) uniform sampler2D lightingDepthTexture;

// main function
void main() {
	// ignore pixels without geometry
	vec4 albedoSample = texture(lightingAlbedoTexture, vUv);

	if (albedoSample.a == 0.0) {
		discard;
	}

	// determine depth
	float depth = texture(lightingDepthTexture, vUv).x;

	// determine world coordinates
	vec4 p = invViewProjUniform * vec4(uvToClipSpace(vUv, depth), 1.0);
	vec3 pos = p.xyz / p.w;

	// determine view direction
	vec3 V = normalize(cameraPosition - pos);

	// determine material data
	Material material;
	material.albedo = toLinear(albedoSample.rgb);
	material.normal = texture(lightingNormalTexture, vUv).xyz * 2.0 - 1.0;
	vec4 data = texture(lightingPbrTexture, vUv);
	material.metallic = data.r;
	material.roughness = data.g;
	material.ao = data.b;

	// total color
	vec3 color = vec3(0.0);

	// process directional light (if required)
	if (hasDirectionalLighting) {
		DirectionalLight light;
		light.L = normalize(directionalLightDirection);
		light.color = directionalLightColor;
		light.ambient = directionalLightAmbient;
		light.shadow = getShadow(pos, (viewUniform * vec4(pos, 1.0)).xyz, dot(material.normal, light.L));

		color += directionalLightPBR(material, light, V);
	}

	// process image based lighting (if required)
	if (hasImageBasedLighting) {
		color += imageBasedLightingPBR(material, V, iblEnvLevels, iblBrdfLut, iblIrradianceMap, iblEnvironmentMap);
	}

	// finalize color (tonemapping and gamma correction are done during post-processing)
	vec3 emissive = texture(lightingEmissiveTexture, vUv).rgb;
	fragColor = vec4(color + toLinear(emissive), 1.0);
	gl_FragDepth = depth;
}
