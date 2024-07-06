//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_godrays[3];
#define u_lightUv u_godrays[0].xy
#define u_lightColor u_godrays[1]
#define u_decay u_godrays[2].x
#define u_exposure u_godrays[2].y
#define u_density u_godrays[2].z
#define u_weight u_godrays[2].w

SAMPLER2D(s_postProcessTexture, 0);
SAMPLER2D(s_occlusionTexture, 1);

void main() {
	int samples = 50;

	vec2 tc = v_texcoord0;
	vec2 deltaTexCoord = (tc - u_lightUv) * u_density / float(samples);
	float illuminationDecay = 1.0;
	float ray = 0.0;

	for (int i = 0; i < samples; i++) {
		tc -= deltaTexCoord;
		ray += texture2D(s_occlusionTexture, tc).r * illuminationDecay * u_weight;
		illuminationDecay *= u_decay;
	}

	gl_FragColor = texture2D(s_postProcessTexture, v_texcoord0) + u_lightColor * ray * u_exposure;
}
