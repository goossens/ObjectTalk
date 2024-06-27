//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position, v_texcoord0

#include <bgfx_shader.glsl>

uniform mat3 u_scissorMat;
uniform mat3 u_paintMat;
uniform vec4 u_innerCol;
uniform vec4 u_outerCol;
uniform vec4 u_scissorExtScale;
uniform vec4 u_extentRadius;
uniform vec4 u_params;

SAMPLER2D(s_tex, 0);

#define u_scissorExt   (u_scissorExtScale.xy)
#define u_scissorScale (u_scissorExtScale.zw)
#define u_extent       (u_extentRadius.xy)
#define u_radius       (u_extentRadius.z)
#define u_feather      (u_params.x)
#define u_strokeMult   (u_params.y)
#define u_texType      (u_params.z)
#define u_type         (u_params.w)

float sdroundrect(vec2 pt, vec2 ext, float rad) {
	vec2 ext2 = ext - vec2(rad, rad);
	vec2 d = abs(pt) - ext2;
	return min(max(d.x, d.y), 0.0) + length(max(d, 0.0) ) - rad;
}

float scissorMask(vec2 p) {
	vec2 sc = abs(mul(u_scissorMat, vec3(p, 1.0) ).xy) - u_scissorExt;
	sc = vec2(0.5, 0.5) - sc * u_scissorScale;
	return clamp(sc.x, 0.0, 1.0) * clamp(sc.y, 0.0, 1.0);
}

float strokeMask(vec2 texcoord) {
	return min(1.0, (1.0 - abs(texcoord.x*2.0 - 1.0) ) * u_strokeMult) * min(1.0, texcoord.y);
}

void main() {
	vec4 result;
	float scissor = scissorMask(v_position);
	float strokeAlpha = strokeMask(v_texcoord0);

	// calculate gradient color using box gradient
	if (u_type == 0.0) {
		vec2 pt = mul(u_paintMat, vec3(v_position, 1.0) ).xy;
		float d = clamp( (sdroundrect(pt, u_extent, u_radius) + u_feather*0.5) / u_feather, 0.0, 1.0);
		vec4 color = mix(u_innerCol, u_outerCol, d);
		color *= strokeAlpha * scissor;
		result = color;

	// calculate color from texture
	} else if (u_type == 1.0) {
		vec2 pt = mul(u_paintMat, vec3(v_position, 1.0) ).xy / u_extent;
		vec4 color = texture2D(s_tex, pt);
		if (u_texType == 1.0) color = vec4(color.xyz * color.w, color.w);
		if (u_texType == 2.0) color = color.xxxx;
		color *= u_innerCol;
		color *= strokeAlpha * scissor;
		result = color;

	// stencil fill
	} else if (u_type == 2.0) {
		result = vec4(1.0, 1.0, 1.0, 1.0);

	// textured tris
	} else if (u_type == 3.0) {
		vec4 color = texture2D(s_tex, v_texcoord0.xy);
		if (u_texType == 1.0) color = vec4(color.xyz * color.w, color.w);
		if (u_texType == 2.0) color = color.xxxx;
		color *= scissor;
		result = color * u_innerCol;
	}

	gl_FragColor = result;
}
