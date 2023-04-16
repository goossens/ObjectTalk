//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx.glsl>

uniform vec4 u_postProcess;
#define u_bloomEnabled bool(u_postProcess.x)
#define u_exposure u_postProcess.y

SAMPLER2D(s_postProcessTexture, 0);
SAMPLER2D(s_bloomTexture, 1);

void main() {
	// sample color
	vec3 color = texture2D(s_postProcessTexture, v_texcoord0).rgb;

	// apply exposure
	color *= u_exposure;

	// add bloom (if required)
	if (u_bloomEnabled) {
		color += texture2D(s_bloomTexture, v_texcoord0).rgb;
	}

	// HDR tonemapping (ACES Knarkowicz)
	color = saturate((color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14));

	// gamma correction
	color = pow(color, vec3_splat(1.0 / 2.2));

	// store final result
	gl_FragColor = vec4(color, 1.0);
}
