//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting


#ifndef OT_PBR_GLSL
#define OT_PBR_GLSL


//	generate pseudo random number
float random(vec2 uv) {
	return fract(sin(dot(uv.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

// Generate a noise value based on a coordinate
float noise(vec2 uv) {
	vec2 uv_index = floor(uv);
	vec2 uv_fract = fract(uv);

	// Four corners in 2D of a tile
	float a = random(uv_index);
	float b = random(uv_index + vec2(1.0, 0.0));
	float c = random(uv_index + vec2(0.0, 1.0));
	float d = random(uv_index + vec2(1.0, 1.0));

	vec2 blur = smoothstep(0.0, 1.0, uv_fract);

	return mix(a, b, blur.x) +
		(c - a) * (1.0 - blur.x) * blur.y +
		(d - b) * blur.x * blur.y;
}

// Fractal Brownian Motion (FBM) data
struct FBM {
	int octaves;
	float amplitude;
	float frequency;
	float lacunarity;
	float persistence;
};

// Calculate Fractal Brownian Motion (FBM)
float fbm(vec2 uv) {
	int octaves = 6;
	float amplitude = 0.5;
	float frequency = 3.0;
	float value = 0.0;

	for(int i = 0; i < octaves; i++) {
		value += amplitude * noise(frequency * uv);
		amplitude *= 0.5;
		frequency *= 2.0;
	}

	return value;
}


#endif
