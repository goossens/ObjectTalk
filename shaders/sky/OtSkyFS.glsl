//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

$input v_position

#include <bgfx_shader.glsl>

uniform vec4 u_sky[3];

#define u_time u_sky[0].x
#define u_cirrus u_sky[0].y
#define u_cumulus u_sky[0].z

#define u_br u_sky[1].x
#define u_bm u_sky[1].y
#define u_g u_sky[1].z

#define u_sunPosition u_sky[2].xyz

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
	CONST(mat3) m = mat3(0.0, 1.60, 1.20, -1.6, 0.72, -0.96, -1.2, -0.96, 1.28);

	float f = 0.0;
	f += noise(p) / 2.0; p = mul(m, p * 1.1);
	f += noise(p) / 4.0; p = mul(m, p * 1.2);
	f += noise(p) / 6.0; p = mul(m, p * 1.3);
	f += noise(p) / 12.0; p = mul(m, p * 1.4);
	f += noise(p) / 24.0;
	return f;
}

void main() {
	// normalize the position
	vec3 pos = normalize(v_position);

	// render just a bit below the horizon
	if (pos.y < -0.1) {
		discard;
	}

	// atmospheric scattering
	CONST(vec3) nitrogen = vec3(0.650, 0.570, 0.475);
	CONST(vec3) nitrogen1 = vec3(0.179, 0.106, 0.051); // pow(nitrogen, 4.0)
	CONST(vec3) nitrogen2 = vec3(0.696, 0.624, 0.535); // pow(nitrogen, 0.84)

	vec3 Kr = u_br / nitrogen1;
	vec3 Km = u_bm / nitrogen2;

	float mu = dot(pos, normalize(u_sunPosition));
	float rayleigh = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu);
	vec3 mie = (Kr + Km * (1.0 - u_g * u_g) / (2.0 + u_g * u_g) / pow(1.0 + u_g * u_g - 2.0 * u_g * mu, 1.5)) / (u_br + u_bm);

	vec3 day_extinction = exp(-exp(-((pos.y + u_sunPosition.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / u_br) * (exp(-pos.y * 16.0) + 0.1) * Kr / u_br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
	vec3 night_extinction = vec3_splat(1.0 - exp(u_sunPosition.y)) * 0.2;
	vec3 extinction = mix(day_extinction, night_extinction, -u_sunPosition.y * 0.2 + 0.5);
	vec3 color = rayleigh * mie * extinction;

	// cirrus clouds
	if (u_cirrus > 0.0) {
		float density = smoothstep(1.0 - u_cirrus, 1.0, fbm(pos / pos.y * 2.0 + u_time * 0.05)) * 0.3;
		color = mix(color, extinction * 4.0, density * max(pos.y, 0.0));
	}

	// "cumulus" clouds
	if (u_cumulus > 0.0) {
		for (int i = 0; i < 2; i++) {
			float density = smoothstep(1.0 - u_cumulus, 1.0, fbm((0.7 + float(i) * 0.01) * pos / pos.y + u_time * 0.3));
			color = mix(color, extinction * density * 5.0, min(density, 1.0) * max(pos.y, 0.0));
		}
	}

	gl_FragColor = vec4(color, 1.0);
	gl_FragDepth = 1.0;
}
