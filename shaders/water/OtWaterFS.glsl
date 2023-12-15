//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_near, v_far

#include <bgfx_shader.glsl>
#include <pbr.glsl>

uniform vec4 u_water[4];

#define u_level u_water[0].x
#define u_distance u_water[0].y
#define u_resolution u_water[0].zw

#define u_scale u_water[1].x
#define u_size u_water[1].y
#define u_time u_water[1].z

#define u_metallic u_water[2].x
#define u_roughness u_water[2].y
#define u_ao u_water[2].z
#define u_reflectivity u_water[2].w

#define u_color u_water[3].rgb
#define u_refractanceFlag bool(u_water[3].a)

uniform vec4 u_lighting[3];
#define u_cameraPosition u_lighting[0].xyz
#define u_directionalLightDirection u_lighting[1].xyz
#define u_directionalLightColor u_lighting[2].xyz
#define u_directionalLightAmbience u_lighting[2].a

SAMPLER2D(s_normalmapSampler, 0);
SAMPLER2D(s_reflectionSampler, 1);
SAMPLER2D(s_refractionSampler, 2);
SAMPLER2D(s_refractionDepthSampler, 3);

// main program
void main() {
	// clip to visible plane
	float t = (u_level - v_near.y) / (v_far.y - v_near.y);

	if (t < 0.0) {
		discard;
	}

	// get the world and clip space positions
	vec3 worldSpacePos = v_near + t * (v_far - v_near);
	vec4 pos = mul(u_viewProj, vec4(worldSpacePos, 1.0));
	vec3 clipSpacePos = pos.xyz / pos.w;

	// clip to distance
	if (clipSpacePos.z > u_distance) {
		discard;
	}

	// get normal
	vec2 uv = worldSpacePos.xz / u_size * u_scale;
	vec2 uv1 = uv + vec2(u_time / 17.0, u_time / 29.0);
	vec2 uv2 = uv - vec2(u_time / -19.0, u_time / 31.0);
	vec2 uv3 = uv + vec2(u_time / 101.0, u_time / 97.0);
	vec2 uv4 = uv - vec2(u_time / 109.0, u_time / -113.0);

	vec4 noise =
		texture2D(s_normalmapSampler, uv1) +
		texture2D(s_normalmapSampler, uv2) +
		texture2D(s_normalmapSampler, uv3) +
		texture2D(s_normalmapSampler, uv4);

	vec3 normal = normalize((noise.xzy * 0.5 - 1.0) * vec3(1.5, 1.0, 1.5));

	// determine reflection and refraction colors
	uv1 = gl_FragCoord.xy / u_resolution;
	uv2 = vec2(uv1.x, 1.0 - uv1.y);
	vec3 reflectionColor = texture2D(s_reflectionSampler, uv2).rgb;
	vec3 refractionColor = u_refractanceFlag ? texture2D(s_refractionSampler, uv1).rgb : u_color;

	// determine view direction
	vec3 viewDirection = normalize(u_cameraPosition - worldSpacePos);

	// determine water color
	float refractiveFactor = pow(dot(viewDirection, vec3(0.0, 1.0, 0.0)), u_reflectivity);
	vec3 color = mix(reflectionColor, refractionColor, refractiveFactor);

	// PBR data
	PBR pbr;
	pbr.albedo = vec4(color, 1.0);
	pbr.metallic = u_metallic;
	pbr.roughness = u_roughness;
	pbr.emissive = vec3_splat(0.0);
	pbr.ao = u_ao;
	pbr.N = normal;
	pbr.V = viewDirection;
	pbr.L = normalize(u_directionalLightDirection);
	pbr.directionalLightColor = u_directionalLightColor;
	pbr.directionalLightAmbience = u_directionalLightAmbience;

	// apply PBR (tonemapping and Gamma correction are done during post-processing)
	gl_FragColor = applyPBR(pbr);
	gl_FragDepth = clipSpacePos.z;
}
