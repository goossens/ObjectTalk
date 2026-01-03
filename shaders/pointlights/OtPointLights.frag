//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core
#extension GL_GOOGLE_include_directive : require

#define LIGHTING_UNIFORMS 1
#define LIGHTING_SAMPLERS 4
#include "lighting.glsl"

#include "pbr.glsl"

layout(location=0) in vec3 vLightPos;
layout(location=1) in vec4 vColor;

layout(location=0) out vec4 fragColor;

layout(std140, set=3, binding=0) uniform UBO {
	mat4 inverseViewProjectionMatrix;
	float viewWidth;
	float viewHeight;
};

layout(set=2, binding=0) uniform sampler2D lightingAlbedoTexture;
layout(set=2, binding=1) uniform sampler2D lightingNormalTexture;
layout(set=2, binding=2) uniform sampler2D lightingPbrTexture;
layout(set=2, binding=3) uniform sampler2D lightingDepthTexture;

// main function
void main() {
	// determine light and vertex position (in world space)
	vec3 lightPosition = vLightPos;

	vec2 uv = vec2(gl_FragCoord.x / viewWidth, gl_FragCoord.y / viewHeight);
	float depth = texture(lightingDepthTexture, uv).x;
	vec4 ndc = vec4(uv.x * 2.0 - 1.0, uv.y * -2.0 + 1.0, depth, 1.0);

	vec4 world = inverseViewProjectionMatrix * ndc;
	world = world / world.w;
	vec3 vertexPosition = world.xyz;

	// determine light data
	PointLight light;
	light.radius = vColor.a;
	light.color = vColor.rgb;
	light.L = normalize(lightPosition - vertexPosition);
	light.attenuation = smoothAttenuation(distance(lightPosition, vertexPosition), light.radius);
	vec3 color = vec3(0.0);

	if (light.attenuation > 0.0) {
		// determine material data
		Material material;
		material.albedo = toLinear(texture(lightingAlbedoTexture, uv).rgb);
		material.normal = texture(lightingNormalTexture, uv).xyz * 2.0 - 1.0;
		vec4 data = texture(lightingPbrTexture, uv);
		material.metallic = data.r;
		material.roughness = data.g;
		material.ao = data.b;

		// determine view direction
		vec3 V = normalize(cameraPosition - vertexPosition);
		color = pointLightPBR(material, light, V);
	}

	fragColor = vec4(color, 1.0);
}
