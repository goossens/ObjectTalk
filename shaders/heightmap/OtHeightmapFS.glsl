//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Inspired by Joey de Vries' Learn OpenGL:
//	https://learnopengl.com/PBR/Lighting

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <noise.glsl>

// uniforms
uniform vec4 u_heightmap[1];
#define u_frequency u_heightmap[0].x
#define u_amplitide u_heightmap[0].y
#define u_lacunarity u_heightmap[0].z
#define u_gain u_heightmap[0].w

// u_frequency // 1.0
// u_amplitide // 0.5
// u_lacunarity // 2.0
// u_gain // 0.5

// constants
#define OCTAVES 4

// functions
float fbm(vec2 p) {
	float sum = 0.0;
	float frequency = u_frequency;
	float amplitide = u_amplitide;

	for (int i = 0; i < OCTAVES; i++)	{
		sum += amplitide * snoise(p * frequency);
		frequency *= u_lacunarity;
		amplitide *= u_gain;
	}

	return sum;
}

void main() {
	gl_FragColor = vec4(fbm(v_texcoord0), 0.0, 0.0, 1.0);
}
