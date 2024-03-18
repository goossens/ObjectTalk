//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_texcoord0

#include <bgfx_shader.glsl>
#include <utilities.glsl>

uniform vec4 u_fog[2];
#define u_fogDensity u_fog[0].x
#define u_fogColor u_fog[1].rgb

uniform mat4 u_invProjUniform;

SAMPLER2D(s_postProcessTexture, 0);
SAMPLER2D(s_depthTexture, 1);

void main() {
	// determine depth
	float depth = texture2D(s_depthTexture, v_texcoord0).x;
	vec4 p = mul(u_invProjUniform, vec4(uvToClipSpace(v_texcoord0, depth), 1.0));
	vec3 pos = p.xyz / p.w;
	depth = length(pos);

	// determine fog amount
	float d = (u_fogDensity / 100.0) * depth;
	float fogAmount = 1.0 - clamp(exp2(d * d * -1.442695), 0.0, 1.0);

	// apply fog color
	vec3 color = texture2D(s_postProcessTexture, v_texcoord0).rgb;
	gl_FragColor = vec4(mix(color, u_fogColor, fogAmount), 1.0);
}
