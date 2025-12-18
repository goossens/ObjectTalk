//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

#version 450 core

layout(location=0) in vec3 vPosition;
layout(location=0) out vec4 fragColor;

layout(std140, set=3, binding=0) uniform UBO {
	vec3 sunPosition;
	float time;
	float cirrus;
	float cumulus;
	float br;
	float bm;
	float g;
};

float hash(float n) {
	return fract(sin(n) * 43758.5453123);
}

float noise(vec3 x) {
	vec3 f = fract(x);
	float n = dot(floor(x), vec3(1.0, 157.0, 113.0));

	return mix(mix(mix(hash(n +   0.0), hash(n +   1.0), f.x),
				   mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
			   mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
				   mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}

float fbm(vec3 p) {
	mat3 m = mat3(0.0, 1.60, 1.20, -1.6, 0.72, -0.96, -1.2, -0.96, 1.28);

	float f = 0.0;
	f += noise(p) / 2.0; p = m * (p * 1.1);
	f += noise(p) / 4.0; p = m * (p * 1.2);
	f += noise(p) / 6.0; p = m * (p * 1.3);
	f += noise(p) / 12.0; p = m * (p * 1.4);
	f += noise(p) / 24.0;
	return f;
}

void main() {
	// normalize the position
	vec3 pos = normalize(vPosition);

	// render just a bit below the horizon
	if (pos.y < -0.1) {
		discard;
	}

	// atmospheric scattering
	// vec3 nitrogen = vec3(0.650, 0.570, 0.475);
	vec3 nitrogen1 = vec3(0.179, 0.106, 0.051); // pow(nitrogen, 4.0)
	vec3 nitrogen2 = vec3(0.696, 0.624, 0.535); // pow(nitrogen, 0.84)

	vec3 Kr = br / nitrogen1;
	vec3 Km = bm / nitrogen2;

	float mu = dot(pos, normalize(sunPosition));
	float rayleigh = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu);
	vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (br + bm);

	vec3 day_extinction = exp(-exp(-((pos.y + sunPosition.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / br) * (exp(-pos.y * 16.0) + 0.1) * Kr / br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
	vec3 night_extinction = vec3(1.0 - exp(sunPosition.y)) * 0.2;
	vec3 extinction = mix(day_extinction, night_extinction, -sunPosition.y * 0.2 + 0.5);
	vec3 color = rayleigh * mie * extinction;

	// cirrus clouds
	if (cirrus > 0.0) {
		float density = smoothstep(1.0 - cirrus, 1.0, fbm(pos / pos.y * 2.0 + time * 0.05)) * 0.3;
		color = mix(color, extinction * 4.0, density * max(pos.y, 0.0));
	}

	// "cumulus" clouds
	if (cumulus > 0.0) {
		for (int i = 0; i < 2; i++) {
			float density = smoothstep(1.0 - cumulus, 1.0, fbm((0.7 + float(i) * 0.01) * pos / pos.y + time * 0.3));
			color = mix(color, extinction * density * 5.0, min(density, 1.0) * max(pos.y, 0.0));
		}
	}

	fragColor = vec4(color, 1.0);
}
