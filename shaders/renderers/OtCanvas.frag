//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450

layout(location=0) in vec2 vPosition;
layout(location=1) in vec2 vUv;

layout(location=0) out vec4 fragColor;

layout(std140, set=3, binding=0) uniform UBO {
	mat3 scissorMat;
	mat3 paintMat;
	vec4 innerCol;
	vec4 outerCol;
	vec2 scissorExt;
	vec2 scissorScale;
	vec2 extent;
	float radius;
	float feather;
	float strokeMult;
	float strokeThr;
	int texType;
	int type;
};

layout(set=0, binding=0) uniform sampler2D tex;

const int simpleShader = 0;
const int fillGradientShader = 1;
const int fillTextureShader = 2;
const int textureShader = 3;

const int transparentTexture = 0;
const int rTexture = 1;
const int rgbaTexture = 2;

float sdroundrect(vec2 pt, vec2 ext, float rad) {
	vec2 ext2 = ext - vec2(rad, rad);
	vec2 d = abs(pt) - ext2;
	return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rad;
}

float scissorMask(vec2 p) {
	vec2 sc = (abs((scissorMat * vec3(p, 1.0)).xy) - scissorExt);
	sc = vec2(0.5, 0.5) - sc * scissorScale;
	return clamp(sc.x, 0.0, 1.0) * clamp(sc.y, 0.0, 1.0);
}

float strokeMask() {
	return min(1.0, (1.0 - abs(vUv.x * 2.0 - 1.0)) * strokeMult) * vUv.y;
}

void main(void) {
	if (type == simpleShader) {
		fragColor = vec4(1.0);

	} else {
		float scissor = scissorMask(vPosition);

		if (type == textureShader) {
			vec4 color = texture(tex, vUv);
			color = texType == 1 ? vec4(color.r) :	vec4(color.rgb * color.a, color.a);
			fragColor = color * innerCol * scissor;

		} else {
			float strokeAlpha = strokeMask();

			if (strokeAlpha < strokeThr) {
				discard;
			}

			if (type == fillGradientShader) {
				// calculate gradient color
				vec2 pt = (paintMat * vec3(vPosition, 1.0)).xy;
				float d = clamp((sdroundrect(pt, extent, radius) + feather * 0.5) / feather, 0.0, 1.0);
				fragColor = mix(innerCol, outerCol, d) * scissor * strokeAlpha;

			} else if (type == fillTextureShader) {
				// calculate color from texture
				vec2 pt = (paintMat * vec3(vPosition, 1.0)).xy / extent;
				vec4 color = texture(tex, pt);
				color = texType == 1 ? vec4(color.r) :	vec4(color.rgb * color.a, color.a);
				fragColor = color * innerCol * scissor * strokeAlpha;
			}
		}
	}
}
