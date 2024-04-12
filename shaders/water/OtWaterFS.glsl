//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_near, v_far

#include <bgfx_shader.glsl>
#include <lighting.glsl>
#include <pbr.glsl>
#include <shadow.glsl>
#include <utilities.glsl>

// uniforms
uniform vec4 u_water[4];

#define u_level u_water[0].x
#define u_distance u_water[0].y

#define u_scale u_water[1].x
#define u_size u_water[1].y
#define u_time u_water[1].z

#define u_metallic u_water[2].x
#define u_roughness u_water[2].y
#define u_ao u_water[2].z
#define u_reflectivity u_water[2].w

#define u_color u_water[3].rgb
#define u_refractanceFlag bool(u_water[3].a)

// texture samplers
SAMPLER2D(s_normalmapTexture, 0);
SAMPLER2D(s_reflectionTexture, 1);
SAMPLER2D(s_refractionTexture, 2);
SAMPLER2D(s_refractionDepthTexture, 3);

// main program
void main() {
	// clip to visible plane
	float t = (u_level - v_near.y) / (v_far.y - v_near.y);

	if (t < 0.0) {
		discard;
	}

	// get the world and NDC positions
	vec3 waterWorldPos = v_near + t * (v_far - v_near);
	vec4 waterClipPos = mul(u_viewProj, vec4(waterWorldPos, 1.0));
	vec3 waterNdcPos = waterClipPos.xyz / waterClipPos.w;

	// clip to distance
	if (waterNdcPos.z > u_distance) {
		discard;
	}

	// get normal
	vec2 uv = waterWorldPos.xz * u_scale;
	vec2 uv1 = (uv / 103.0) + vec2(u_time / 17.0, u_time / 29.0);
	vec2 uv2 = (uv / 107.0) - vec2(u_time / -19.0, u_time / 31.0) + vec2_splat(0.23);
	vec2 uv3 = (uv / vec2(897.0, 983.0)) + vec2(u_time / 101.0, u_time / 97.0) + vec2_splat(0.51);
	vec2 uv4 = (uv / vec2(991.0, 877.0)) - vec2(u_time / 109.0, u_time / -113.0) + vec2_splat(0.71);

	vec4 noise =
		texture2D(s_normalmapTexture, uv1) +
		texture2D(s_normalmapTexture, uv2) +
		texture2D(s_normalmapTexture, uv3) +
		texture2D(s_normalmapTexture, uv4);

	noise = noise * 0.5 - 1.0;

	float dist = length(u_cameraPosition - waterWorldPos);
	vec3 normal = normalize(noise.xzy * vec3(2.0, clamp(dist * 0.001, 1.0, 100.0), 2.0));

	// determine reflection and refraction colors
	vec2 refractionUv = gl_FragCoord.xy / u_viewRect.zw;
	vec2 reflectionUv = vec2(refractionUv.x, 1.0 - refractionUv.y);
	vec3 reflectionColor = texture2D(s_reflectionTexture, reflectionUv).rgb;
	vec3 refractionColor = u_refractanceFlag ? texture2D(s_refractionTexture, refractionUv).rgb : u_color;

	// determine view direction and water depth
	vec3 viewDirection = normalize(u_cameraPosition - waterWorldPos);

	// determine water depth
	float refractionDepth = texture2D(s_refractionDepthTexture, refractionUv).r;
	vec3 refractionWorldPos = uvToWorldSpace(gl_FragCoord.xy / u_viewRect.zw, refractionDepth);
	float waterDepth = length(refractionWorldPos - waterWorldPos);

	// determine water color and transparency
	float refractiveFactor = pow(dot(viewDirection, vec3(0.0, 1.0, 0.0)), u_reflectivity);
	vec3 color = mix(reflectionColor, refractionColor, refractiveFactor);
	float alpha = clamp((waterDepth / 1.0), 0.0, 1.0);

	// material data
	Material material;
	material.albedo = vec4(color, alpha);
	material.normal = normal;
	material.metallic = u_metallic;
	material.roughness = u_roughness;
	material.ao = u_ao;

	// light data
	DirectionalLight light;
	light.L = normalize(u_directionalLightDirection);
	light.color = u_directionalLightColor;
	light.ambience = u_directionalLightAmbience;

	// determine view direction
	vec3 V = normalize(u_cameraPosition - waterWorldPos);

	// total color
	color = vec3_splat(0.0);

	// process directional light (if required)
	if (u_hasDirectionalLighting) {
		DirectionalLight light;
		light.L = normalize(u_directionalLightDirection);
		light.color = u_directionalLightColor;
		light.ambience = u_directionalLightAmbience;
		light.shadow = getShadow(waterWorldPos, mul(u_view, vec4(waterWorldPos, 1.0)).xyz, dot(material.normal, light.L));

		color += directionalLightPBR(material, light, V);
	}

	// process image based lighting (if required)
	if (u_hasImageBasedLighting) {
		color += imageBasedLightingPBR(material, V, u_iblEnvLevels, s_iblBrdfLut, s_iblIrradianceMap, s_iblEnvironmentMap);
	}

	// set results
	gl_FragColor = vec4(color, 1.0);

#if BGFX_SHADER_LANGUAGE_GLSL
	gl_FragDepth = (waterNdcPos.z + 1.0) * 0.5;
#else
	gl_FragDepth = waterNdcPos.z;
#endif
}
