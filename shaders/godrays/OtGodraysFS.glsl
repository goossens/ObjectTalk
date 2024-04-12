//	ObjectTalk Scripting Language
//	Copyright (c) 2020-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

#define exposure 0.6
#define decay 0.93
#define density 0.96
#define weight 0.4
#define samples 20
#define clampMax 1.0

uniform vec4 u_godrays[1];
#define u_lightPos u_godrays[0].xy

SAMPLER2D(s_texture, 0);
SAMPLER2D(s_bloomTexture, 1);

void main() {
	vec2 deltaTextCoord = vec2(v_texcoord0 - u_lightPos);
	deltaTextCoord *= 1.0 /  float(samples) * density;
	vec2 coord = v_texcoord0;
	float illuminationDecay = 1.0;
	vec3 color = vec3_splat(0.0);

	for (int i = 0; i < samples; i++) {
		coord -= deltaTextCoord;
		color += texture2D(s_texture, coord).rgb * illuminationDecay * weight;
		illuminationDecay *= decay;
	}

	color *= exposure;
	color = clamp(color, 0.0, clampMax) * vec4(1.0, 1.0, 0.94, 1.0);

	vec3 godrays = texture2D(s_bloomTexture, v_texcoord0).rgb;
	gl_FragColor = vec4(clamp(color + godrays, 0.0, 1.0), 1.0f);
}
