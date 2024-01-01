//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by: https://www.shadertoy.com/view/4dlGW2

$input v_texcoord0

#include <bgfx_shader.glsl>

// uniforms
uniform vec4 u_tileableFbm[2];
#define u_frequency u_tileableFbm[0].x
#define u_lacunarity u_tileableFbm[0].y
#define u_amplitude u_tileableFbm[0].z
#define u_persistence u_tileableFbm[0].w
#define u_octaves int(u_tileableFbm[1].x)

// functions
float hash(vec2 p, float scale) {
	return fract(sin(dot(mod(p, scale), vec2(27.16898, 38.90563))) * 5151.5473453);
}

float tileableNoise(vec2 p, float scale) {
	p *= scale;
	vec2 f = fract(p);
	p = floor(p);
	f = f * f * (3.0 - 2.0 * f);

	return mix(
		mix(hash(p, scale), hash(p + vec2(1.0, 0.0), scale), f.x),
		mix(hash(p + vec2(0.0, 1.0), scale), hash(p + vec2(1.0, 1.0), scale), f.x), f.y);
}

void main() {
	float sum = 0.0;
	float frequency = u_frequency;
	float amplitude = u_amplitude;

	for (int i = 0; i < u_octaves; i++)	{
		sum += amplitude * tileableNoise(v_texcoord0, frequency);
		frequency *= u_lacunarity;
		amplitude *= u_persistence;
	}

	gl_FragColor = vec4(sum, 0.0, 0.0, 1.0);
}
