//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Derived from: https://github.com/patriciogonzalezvivo/lygia

#include "bgfx_compute.glsl"

#define THREADS 8

IMAGE2D_WO(outTexture, rgba8, 0);

// constants
#define simplexNoiseType 0
#define perlinNoiseType 1
#define gradientNoiseType 2
#define worleyNoiseType 3

// uniforms
uniform vec4 u_fbm[2];
#define u_frequency u_fbm[0].x
#define u_lacunarity u_fbm[0].y
#define u_amplitude u_fbm[0].z
#define u_persistence u_fbm[0].w
#define u_octaves int(u_fbm[1].x)
#define u_noiseType int(u_fbm[1].y)

// functions
vec2 mod289(vec2 v) {
	return v - floor(v * (1.0 / 289.0)) * 289.0;
}

vec3 mod289(vec3 v) {
	return v - floor(v * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 v) {
	return v - floor(v * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 v) {
	return mod289(((v * 34.0) + 1.0) * v);
}

vec4 permute(vec4 v) {
	return mod289(((v * 34.0) + 1.0) * v);
}

vec4 taylorInvSqrt(vec4 v) {
	return 1.79284291400159 - 0.85373472095314 * v;
}

vec2  cubic(vec2 v) {
	return v * v * (3.0 - 2.0 * v);
}

vec2 quintic(vec2 v) {
	return v * v * v * (v * (v * 6.0 - 15.0) + 10.0);
}

float random(vec2 uv) {
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 random2(vec2 p) {
	vec3 p3 = fract(p.xyx * vec3(0.1031, 0.1030, 0.0973));
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.xx + p3.yz) * p3.zy);
}

float simplexNoise(vec2 uv) {
	const vec4 C = vec4(
		0.211324865405187,		// (3.0 - sqrt(3.0)) / 6.0
		0.366025403784439,		// 0.5 * (sqrt(3.0) - 1.0)
		-0.577350269189626,		// -1.0 + 2.0 * C.x
		0.024390243902439);		// 1.0 / 41.0

	// first corner
	vec2 i  = floor(uv + dot(uv, C.yy));
	vec2 x0 = uv - i + dot(i, C.xx);

	// other corners
	vec2 i1;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;

	// permutations
	i = mod289(i);
	vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
	m = m * m;
	m = m * m;

	// gradients: 41 points uniformly over a line, mapped onto a diamond.
	// the ring size 17 * 17 = 289 is close to a multiple of 41 (41 * 7 = 287)
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;

	// normalise gradients implicitly by scaling m
	// approximation of: m *= inversesqrt(a0*a0 + h * h);
	m *= 1.79284291400159 - 0.85373472095314 * ( a0 * a0 + h * h);

	// compute final noise value at uv
	vec3 g;
	g.x  = a0.x * x0.x + h.x * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return ((130.0 * dot(m, g)) + 1.0) / 2.0;
}

float perlinNoise(vec2 uv) {
	vec4 Pi = floor(uv.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
	vec4 Pf = fract(uv.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
	Pi = mod289(Pi);

	vec4 ix = Pi.xzxz;
	vec4 iy = Pi.yyww;
	vec4 fx = Pf.xzxz;
	vec4 fy = Pf.yyww;

	vec4 i = permute(permute(ix) + iy);

	vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
	vec4 gy = abs(gx) - 0.5 ;
	vec4 tx = floor(gx + 0.5);
	gx = gx - tx;

	vec2 g00 = vec2(gx.x,gy.x);
	vec2 g10 = vec2(gx.y,gy.y);
	vec2 g01 = vec2(gx.z,gy.z);
	vec2 g11 = vec2(gx.w,gy.w);

	vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
	g00 *= norm.x;
	g01 *= norm.y;
	g10 *= norm.z;
	g11 *= norm.w;

	float n00 = dot(g00, vec2(fx.x, fy.x));
	float n10 = dot(g10, vec2(fx.y, fy.y));
	float n01 = dot(g01, vec2(fx.z, fy.z));
	float n11 = dot(g11, vec2(fx.w, fy.w));

	vec2 fade_xy = quintic(Pf.xy);
	vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
	float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
	return ((2.3 * n_xy) + 1.0) / 2.0;
}

float gradientNoise(vec2 uv) {
	vec2 i = floor(uv);
	vec2 f = fract(uv);
	float a = random(i);
	float b = random(i + vec2(1.0, 0.0));
	float c = random(i + vec2(0.0, 1.0));
	float d = random(i + vec2(1.0, 1.0));
	vec2 u = cubic(f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float worleyNoise(vec2 p) {
	vec2 n = floor(p);
	vec2 f = fract(p);

	float dis = 1.0;
	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			vec2 g = vec2(i, j);
			vec2 o = random2(n + g);
			vec2 delta = g + o - f;
			float d = length(delta);
			dis = min(dis, d);
		}
	}

	return 1.0 - dis;
}

float noise(vec2 uv) {
	if (u_noiseType == simplexNoiseType) {
		return simplexNoise(uv);

	} else if (u_noiseType == perlinNoiseType) {
		return perlinNoise(uv);

	} else if (u_noiseType == gradientNoiseType) {
		return gradientNoise(uv);

	} else if (u_noiseType == worleyNoiseType) {
		return worleyNoise(uv);

	} else {
		return 0.0;
	}
}

NUM_THREADS(THREADS, THREADS, 1)
void main() {
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(outTexture);

	if (coord.x >= size.x || coord.y >= size.y) {
		return;
	}

	vec2 uv = (vec2(coord) + 0.5) / vec2(size);

	float sum = 0.0;
	float denom = 0.0;
	float frequency = u_frequency;
	float amplitude = u_amplitude;

	for (int i = 0; i < u_octaves; i++)	{
		sum += amplitude * noise(frequency * uv);
		denom += amplitude;

		frequency *= u_lacunarity;
		amplitude *= u_persistence;
	}

	imageStore(outTexture, coord, vec4(sum / denom, 0.0, 0.0, 1.0));
}
