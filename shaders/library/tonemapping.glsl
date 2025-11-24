//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#ifndef OT_TONE_MAPPING_GLSL
#define OT_TONE_MAPPING_GLSL


vec3 tonemapReinhardSimple(vec3 color) {
	// Photographic Tone Reproduction For Digital Images" by Reinhard et al. 2002
	// https://www.researchgate.net/publication/2908938_Photographic_Tone_Reproduction_For_Digital_Images
	return color / (color + vec3(1.0));
}

vec3 tonemapReinhardExtended(vec3 color) {
	// Photographic Tone Reproduction For Digital Images" by Reinhard et al. 2002
	// https://www.researchgate.net/publication/2908938_Photographic_Tone_Reproduction_For_Digital_Images
	return (color * (vec3(1.0) + (color / (4.0 * 4.0))) / (vec3(1.0) + color));
}

vec3 tonemapFilmic(vec3 color) {
	// Filmic Tonemapping Operators
	// http://filmicworlds.com/blog/filmic-tonemapping-operators/
	color = max(vec3(0.0), color - 0.004);
	color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
	return pow(color, vec3(2.2));
}

vec3 tonemapAcesFilmic(vec3 color) {
	// ACES Filmic Tone Mapping Curve, Krzysztof Narkowicz 2015
	// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
	return clamp((color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14),0.0, 1.0);
}

vec3 tonemapUncharted2(vec3 color) {
	// Uncharted 2 filmic operator, John Hable 2010
	// http://filmicworlds.com/blog/filmic-tonemapping-operators/
	// values used are slightly modified, comments have the original values
	float whiteScale = 0.72513;
	float ExposureBias = 2.0;

	float A = 0.22; // shoulder strength // 0.15
	float B = 0.30; // linear strength // 0.50
	float C = 0.10; // linear angle
	float D = 0.20; // toe strength
	float E = 0.01; // toe numerator // 0.02
	float F = 0.30; // toe denominator

	color *= ExposureBias;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	return color / whiteScale;
}

vec3 tonemapLottes(vec3 color) {
	// Advanced Techniques and Optimization of HDR Color Pipelines, Timothy Lottes 2016
	// https://gpuopen.com/wp-content/uploads/2016/03/GdcVdrLottes.pdf
	vec3 a = vec3(1.6);
	vec3 d = vec3(0.977);
	vec3 hdrMax = vec3(8.0);
	vec3 midIn = vec3(0.18);
	vec3 midOut = vec3(0.267);

	vec3 b = (-pow(midIn, a) + pow(hdrMax, a) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

	vec3 c = (pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

  return pow(color, a) / (pow(color, a * d) * b + c);
}

vec3 tonemapUchimura(vec3 color) {
	// HDR theory and practice, Hajime Uchimura, 2017
	// https://www.desmos.com/calculator/gslcdxvipg
	float P = 1.0;  // max display brightness
	float a = 1.0;  // contrast
	float m = 0.22; // linear section start
	float l = 0.4;  // linear section length
	float c = 1.33; // black
	float b = 0.0;  // pedestal

	float l0 = ((P - m) * l) / a;
	// float L0 = m - m / a;
	// float L1 = m + (1.0 - m) / a;
	float S0 = m + l0;
	float S1 = m + a * l0;
	float C2 = (a * P) / (P - S1);
	float CP = -C2 / P;

	vec3 w0 = 1.0 - smoothstep(0.0, m, color);
	vec3 w2 = step(m + l0, color);
	vec3 w1 = 1.0 - w0 - w2;

	vec3 T = m * pow(color / m, vec3(c)) + b;
	vec3 S = P - (P - S1) * exp(CP * (color - S0));
	vec3 L = m + a * (color - m);

	return T * w0 + L * w1 + S * w2;
}

#endif
