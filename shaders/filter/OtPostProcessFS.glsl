//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>

uniform vec4 u_postProcess;
#define u_exposure u_postProcess.x
#define u_contrast u_postProcess.y

SAMPLER2D(s_postProcessTexture, 0);

void main() {
	// sample color
	vec3 color = texture2D(s_postProcessTexture, v_texcoord0).rgb;

	// HDR tonemapping (ACES Knarkowicz)
	color = saturate((color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14));

	// gamma correction
	color = pow(color, vec3_splat(1.0 / 2.2));

	// apply exposure and contrast
	color = ((color - 0.5) * u_contrast + 0.5) * u_exposure;

	// store final result
	gl_FragColor = vec4(color, 1.0);
}
