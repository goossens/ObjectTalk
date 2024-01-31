//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

// constants
#define squareBumpFunction 0
#define euclideanFunction 1
#define euclidean2Function 2
#define hyperboloidFunction 3
#define diagonalFunction 4

#define C 0.2

// uniforms
uniform vec4 u_islander;
#define u_distanceFunction int(u_islander.x)

// samplers
SAMPLER2D(s_texture, 0);

float radialDistance(vec2 uv) {
	uv = (uv - 0.5) * 2.0;

	if (u_distanceFunction == squareBumpFunction) {
		return 1.0 - (1.0 - uv.x * uv.x) * (1.0 - uv.y * uv.y);

	} else if (u_distanceFunction == euclideanFunction) {
		return sqrt(uv.x * uv.x + uv.y * uv.y) / 1.414213562373095;

	} else if (u_distanceFunction == euclidean2Function) {
		return (uv.x * uv.x + uv.y * uv.y) / 2.0;

	} else if (u_distanceFunction == hyperboloidFunction) {
		return (sqrt(uv.x * uv.x + uv.y * uv.y + C * C) - C) / (sqrt(1.0 + C * C) - C);

	} else if (u_distanceFunction == diagonalFunction) {
		return max(abs(uv.x), abs(uv.y));

	} else {
		return 1.0;
	}
}

void main() {
	float distance = radialDistance(v_texcoord0);
	float height = texture2D(s_texture, v_texcoord0).x;
	height *= 1.0 - distance;
	gl_FragColor = vec4(height, 0.0, 0.0, 1.0);
}
